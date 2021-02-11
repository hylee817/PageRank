#include "LLAMA.h"
#include <fstream>
#include <string>
#include <math.h>
#include <iostream>
#include <sstream>
#include <iterator>
#include <vector>

using namespace std;

LLAMA::LLAMA(){
	snapshots = -1;
}

LLAMA::LLAMA(int max_id, int m_, int p_, bool in){
	snapshots = -1;
	in_llama = in;
	max_sid = max_id;
	m = m_;
	num_pages = p_;
	page_size = pow(2,m);

	i_tables = new i_table[max_id];
	e_tables = new e_table[max_id];

	cout << "llama initalized" << endl;
}

void LLAMA::read(string fname){
	  cout << "Opening " << fname << "..." << endl;
	  ifstream input(fname);
	  if (input.fail()){
		  cout << "Failed to open: " << fname << endl;
		  exit(1);
	  }

	  //temporary
	  //vertices = 4039; edges=88234;
	  //tmp = new vector<int>[vertices];
	  //deg = new int[vertices]();
	  //to be deleted

	  string line;
	  while(!input.eof()){
		  getline(input, line);
		  if (line.empty()) continue;
		  if (line.find("Edges", 0) != string::npos){
			  int M = stoi(line.substr(line.find(":")+1, line.find("Edges")));
			  int N = stoi(line.substr(line.find("Edges")+6));
			  cout << M << " " << N << endl;
			  vertices = M; edges = N;
			  tmp = new vector<int>[M];
			  deg = new int[M]();
		  }
		  if (line.at(0) == '#') continue; //skip comments
		  int from,to;
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
		  
	  }input.close();
	  cout << "Reading " << fname << " DONE" << endl;

}

int LLAMA::load(){
	int id = create_snapshot();
	cout << "Snapshot " << id << " initialized" << endl;

	int e_iter = 0;
	for (int id=0;id<vertices;id++){
		pair<uint32_t, uint32_t> vtx_addr = convert(id);
		uint32_t page = vtx_addr.first;
		uint32_t entry = vtx_addr.second;
		//cout << page << ", " << entry << endl;
		i_tables[snapshots][page][entry].sid = snapshots;
		i_tables[snapshots][page][entry].offset = e_iter;
		i_tables[snapshots][page][entry].len = tmp[id].size();
		//if (id < 100){
		//cout << id << " " << e_iter << " " << tmp[id].size() << endl;}
		for (int i=0;i<tmp[id].size();i++){
			e_tables[snapshots][e_iter] = tmp[id][i];
			e_iter += 1;
			if (e_iter % 10000 == 0){cout << e_iter << "/" << edges <<endl;}
		}
	}

	cout << "Snapshot " << id << " DONE" << endl;
	return id;
}

//todo: if empty snapshot  -> pt to prev. i_table
int LLAMA::create_snapshot(){

	i_tables[snapshots+1] = new page[num_pages];

	if (snapshots >= 0){
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
	uint32_t page = converted.first;
	uint32_t entry = converted.second;

	//cout << "page: " << page << ", entry: " << entry << endl;
	vertex cur = i_tables[snapshots][page][entry];
	int sid = cur.sid;
	int offset = cur.offset;
	//cout << sid << ", " << offset << endl;

	vector<int> neigh;

	int i = 0;
	while (i < cur.len){
		int val = e_tables[sid][offset+i];
		if (val < 0){
			offset = e_tables[sid][offset+i+1];
			sid = -val;
			continue;
		}
		neigh.push_back(val);
		i += 1;
	}

	return neigh;
}

pair<uint32_t, uint32_t> LLAMA::convert(int vid){

	uint64_t vt_idx = vid;

	uint32_t page = vt_idx >> m;
	uint32_t entry = vt_idx & ((1 << m) -1);

	return make_pair(page, entry);
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
			cout << v << " ";
		}cout << endl;
	}
	
}



