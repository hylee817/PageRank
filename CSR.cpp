#include "CSR.h"
#include <iostream>
using namespace std;

CSR::CSR(){
	v_ptr = 0;
	e_ptr = 0;
}

void CSR::set(int v, int e) {
	vertices = v;
	edges = e;
	
	idx = new int[v + 1];
	idx[0] = 0;
	val = new int[e];
}


void CSR::add(int from, int to){

	if (v_ptr < from){ //next id
		for(int d=v_ptr+1;d<=from;d++){
			idx[d] = idx[v_ptr+1]; //extend
		}
		v_ptr = from;
	} //v_ptr -> current vtx id

	//add edge
	val[e_ptr] = to;
	e_ptr += 1; //pts to next empty slot
	idx[v_ptr+1] = e_ptr; //end (exclude)
	
	//print progress
	if (e_ptr % 10000000 == 0){
		cout << e_ptr << "/" << edges << " done" << endl;
	}
}

void CSR::end(){
	
	if (v_ptr < vertices){
		int d;
		for(d=v_ptr+1;d<=vertices;d++){
			idx[d] = idx[v_ptr+1]; //extend
		}
		v_ptr = d;
	}

	cout << "INPUT GRAPH DONE" << endl;
	print_state();
}

int CSR::at(int from, int seq){
	
	if (from >= vertices){
		cout << "request for value" << from << " out of range: " << vertices << endl;
		exit(1);
	}
	else if(idx[from] + seq >= idx[from+1] ){
		cout << "request for value" << idx[from] + seq << "out of range: " << idx[from+1] << endl; 
	}
	else{
		return val[idx[from]+seq];
	}
}

int CSR::out_deg(int from){
	if (from >= vertices){
		cout << "request for value" << from << " out of range: " << vertices << endl;
		exit(1);
	}
	else{
		return idx[from+1]-idx[from];
	}
}

void CSR::print_state(){
	cout << "V at: " << v_ptr << endl;
	cout << "E at: " << e_ptr << endl;
}

void CSR::print_idx(){
	for(int i=0; i<100;i++){
		cout << i << " " << idx[i] << endl;
	}
}

void CSR::print_val(){
	for(int i=0; i<edges;i++){
		cout << i << " " << val[i] << endl;
	}
}

int CSR::get_v(){
	return v_ptr;
}

int CSR::get_e(){
	return e_ptr;
}
