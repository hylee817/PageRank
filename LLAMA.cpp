#include "LLAMA.h"
#include <fstream>
#include <string>
#include <math.h>
#include <iostream>
#include <sstream>
#include <iterator>
#include <vector>

using namespace std;

LLAMA::LLAMA(){}
LLAMA::LLAMA(int max_id, int m_, int p_, int v_, int e_, bool in_){
	snapshots = 0;

	in_llama = in_;
	max_sid = max_id;
	m = m_;
	num_pages = p_;
	page_size = pow(2,m);
	vertices = v_; 
	edges = e_;

	i_tables = new i_table[max_id];
	e_tables = new e_table[max_id];

	cout << "LLAMA created" << endl;
}

void LLAMA::read(string fname){
	  cout << "Opening " << fname << "..." << endl;
	  ifstream input(fname);
	  if (input.fail()){
		  cout << "Failed to open: " << fname << endl;
		  exit(1);
	  }

	  tmp = new vector<int>[vertices];
	  deg = new int[vertices]();

	  int global_iter = 0;
	  int local_iter = 0;
	  string line;
	  while(!input.eof()){
		  getline(input, line);
		  if (line.empty()) continue;
		  if (line.at(0) == '#') continue; //skip comments
		  if (global_iter%(edges / max_sid) == 0 && global_iter != 0) {

			  //store as new snapshot
			  int cur_id = load(local_iter);

			  //reset storage
			  tmp = new vector<int>[vertices];
			  local_iter = 0;
		  }

		  int from, to;
		  stringstream ss(line);
		  ss >> from; ss >> to;

		  if (in_llama){
			  tmp[to].push_back(from);
			  deg[from] += 1; //outgoing degree
		  }
		  else{
			  tmp[from].push_back(to);
			  deg[to] += 1; //incoming degree
		  }
		  global_iter += 1;
		  local_iter += 1;
	  }input.close();
	  cout << "Reading " << fname << " DONE" << endl;

}

int LLAMA::load(int edges_in_snapshot){ //load a single snapshot
	
	int sid = create_snapshot(); //indirection arr. copied
	cout << "Snapshot " << sid << " initialized" << endl;
	cout << edges_in_snapshot << " edges in this Snapshot" << endl;

	//new edge table for this snapshot
	if (sid > 1){e_tables[sid] = new int[2*vertices + edges_in_snapshot];}

	int e_iter = 0; uint32_t prev_pg = 1;
	for (int vid=0;vid<vertices;vid++){
		pair<uint32_t, uint32_t> vtx_addr = convert(vid);
		uint32_t pg = vtx_addr.first;
		uint32_t ent = vtx_addr.second;

		if (sid > 1){ 
			//skip if this vtx hasn't changed
			if (tmp[vid].size() == 0){continue;}
			//fetch vertex info from prev. snapshot
			vertex prev = i_tables[sid][pg][ent];

			if(pg != prev_pg){
			//copy page (deep copy)
			page new_page = new vertex[page_size];
			for (int i=0;i<page_size;i++){
				new_page[i].sid = i_tables[sid][pg][i].sid;
				new_page[i].len = i_tables[sid][pg][i].len;
				new_page[i].offset = i_tables[sid][pg][i].offset;
			}
			//update indirection array to point new page
			i_tables[sid][pg] = new_page;
			}prev_pg = pg;

			//update page info
			i_tables[sid][pg][ent].sid = sid;
			i_tables[sid][pg][ent].offset = e_iter;
			i_tables[sid][pg][ent].len =  prev.len + tmp[vid].size();
			//cout << sid << ", " << e_iter << ", " << tmp[vid].size() << endl;


			//update edge table
			for (int i=0;i<tmp[vid].size();i++){
				e_tables[sid][e_iter++] = tmp[vid][i];
			}//add continuation record
			e_tables[sid][e_iter++] = -prev.sid;
			e_tables[sid][e_iter++] = prev.offset;
		}
		else{
			//update vertex info
			i_tables[sid][pg][ent].sid = sid;
			i_tables[sid][pg][ent].offset = e_iter;
			i_tables[sid][pg][ent].len = tmp[vid].size();
			//cout << sid << ", " << e_iter << ", " << tmp[vid].size() << endl;

			//update edge info
			for (int i=0;i<tmp[vid].size();i++){
				e_tables[sid][e_iter++] = tmp[vid][i];
			}
		}
	}

	cout << "Snapshot " << sid << " DONE" << endl << endl;
	return sid;
}

//todo: if empty snapshot  -> pt to prev. i_table
int LLAMA::create_snapshot(){

	i_tables[snapshots+1] = new page[num_pages];

	if (snapshots > 0){
		cout << "Copying previous indirection array..." << endl;
		//copy prev. indirection table
		for (int i=0;i<num_pages;i++){
			i_tables[snapshots+1][i] = i_tables[snapshots][i];
		}
	}
	else{
		cout << "Loading first snapshot..." << endl;
		//first load (create all structures)
		e_tables[snapshots+1] = new int[edges];
		i_tables[snapshots+1] = new page[num_pages];
		for (int i=0;i<num_pages;i++){
			page new_page = new vertex[page_size];
			i_tables[snapshots+1][i] = new_page;
		}
	}
	snapshots += 1;
	return snapshots;
}

vector<int> LLAMA::neighbors(int vid){

	pair<uint32_t, uint32_t> converted = convert(vid);
	uint32_t pg = converted.first;
	uint32_t ent = converted.second;

	vertex cur = i_tables[snapshots][pg][ent];
	int sid = cur.sid;
	int offset = cur.offset;
	//cout << "id[" << vid << "]: ";

	vector<int> neigh;

	int i = 0; int ctr = 0;
	while (ctr < cur.len){
		int val = e_tables[sid][offset+i];
		//cout << val << ", ";
		if (val < 0){ //snapshot, offset
			offset = e_tables[sid][offset+i+1];
			sid = -val;
			i = 0;
			continue;
		}
		neigh.push_back(val);
		i += 1; ctr += 1;
	}//cout << endl;

	return neigh;
}

pair<uint32_t, uint32_t> LLAMA::convert(int vid){

	uint64_t vt_idx = vid;

	uint32_t pg = vt_idx >> m;
	uint32_t ent = vt_idx & ((1 << m) -1);

	return make_pair(pg, ent);
}

void LLAMA::set(int sid){
	snapshots = sid;
}

void LLAMA::print_(int sid){
	cout << "Printing All..." << endl;
	cout << "in_or_out: " << in_llama << endl;
	cout << "vertices: " << vertices << ", edges: " << edges << endl;
	cout << "num_pages: " << num_pages << ", page_size: " << page_size << endl;
	cout << "Snapshot: " << sid << endl;

	for (int i=0; i<vertices;i++){
		vector<int> neigh = neighbors(i);
		cout << "id [" << i << "]: ";
		for(int v : neigh){
			cout << v << ", ";
		}cout << endl;
	}
	
}



