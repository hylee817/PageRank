#include "INCSR.h"
#include <iostream>
using namespace std;

INCSR::INCSR(){
	v_ptr = 0;
	e_ptr = 0;
}

void INCSR::set(int v, int e) {
	vertices = v;
	edges = e;
	
	tmp = new vector<int>[v];
	out = new int[v]();

	idx = new int[v + 1];
	idx[0] = 0;
	val = new int[e];
}


void INCSR::add(int from, int to){
	tmp[to].push_back(from); //dest based
	out[from] += 1; //update out degree
}

void INCSR::end(){
	cout << "Converting to In_node CSR" << endl;

	//concatenate vectors
	for (int id=0;id<vertices;id++){
		for (int i=0; i < tmp[id].size(); i++){
			val[e_ptr] = tmp[id][i]; //from vtx
			e_ptr += 1;
		}
		idx[id+1] = e_ptr;
	}
	cout << "INPUT GRAPH DONE" << endl;
	print_state();
}

int INCSR::base(int to){
	if (to >= vertices){
		cout << "request for value" << to << "out of range: " << vertices << endl;
		exit(1);
	}
	return idx[to];
}

int INCSR::at(int seq){
	return val[seq];
}

int INCSR::at(int to, int seq){
	
	if (to >= vertices){
		cout << "request for value" << to << " out of range: " << vertices << endl;
		exit(1);
	}
	else if(idx[to] + seq >= idx[to+1] ){
		cout << "request for value" << idx[to] + seq << "out of range: " << idx[to+1] << endl; 
	}
	else{
		return val[idx[to]+seq];
	}
}

int INCSR::in_deg(int to){
	if (to >= vertices){
		cout << "request for value" << to << " out of range: " << vertices << endl;
		exit(1);
	}
	else{
		return idx[to+1]-idx[to];
	}
}

int INCSR::out_deg(int from){
	return out[from];
}

void INCSR::print_state(){
	cout << "V at: " << v_ptr << endl;
	cout << "E at: " << e_ptr << endl;
}

void INCSR::print_idx(){
	for(int i=0; i<100;i++){
		cout << i << " " << idx[i] << endl;
	}
}

void INCSR::print_val(){
	for(int i=0; i<100;i++){
		cout << i << " " << val[i] << endl;
	}
}

int INCSR::get_v(){
	return v_ptr;
}

int INCSR::get_e(){
	return e_ptr;
}
