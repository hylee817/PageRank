#include <iostream>
#include <fstream>
#include <stdexcept>
#include <string>
#include <vector>
#include <math.h>
#include <stdio.h>
#include <sstream>
#define HAVE_STRUCT_TIMESPEC
#include <sys/time.h>
#include <omp.h>
#include "LLAMA.h"
using namespace std;

const float eps = 1e-4;
const float damp = 0.85;
const float beta = 0.8;

/*
a graph in LLAMA consists of:
	multiple edge tables
	a single vertex table shared by all snapshots that maps vertex IDs to per-vertex structures

create snapshot by:
	copying the indirection array
modify page by:
	copy the page
	update the indirection array to reference the new page
	change the new page accordingly
 */

LLAMA llama(1, 10, 4800, true);
//LLAMA llama(1, 5, 128, true);

vector<float> compute_PR(){
	float N = llama.vertices;
	vector<float> pr(N, 1/N);
	vector<float> out(N);
	float base = (1.0f - beta) / N;
	
	int count = 0;
	float diff = 1;
	while(diff > eps){
		count +=1;
		diff = 0;
		#pragma omp parallel for
		for (int id=0; id<llama.vertices;id++)
			out[id] = pr[id] / llama.deg[id];
		#pragma omp parallel for reduction(+ : diff) schedule(dynamic, 64)
		for (int id=0; id<llama.vertices; id++){
			float in_total = 0;
			vector<int> neigh = llama.neighbors(id);
			for(auto v: neigh){
				in_total += out[v];
			}
			float old = pr[id];
			pr[id] = base + beta*in_total;
			diff += fabs(pr[id] - old);
		}
		printf(" %2d    %lf\n", count, diff);
	}
	
	return pr;
}



int main(void){

	llama.read("soc-LiveJournal1.txt");
	//llama.read("facebook_combined.txt");
	int id = llama.load();
	//llama.print_(id);


	struct timeval start,end;
	long secs_used, micros_used;

	gettimeofday(&start, NULL);
	vector<float> pr = compute_PR();
	gettimeofday(&end, NULL);

	secs_used = (end.tv_sec - start.tv_sec);
	micros_used = ((secs_used*1000000) + end.tv_usec) - (start.tv_usec);
	cout << "time(s): " << secs_used << "(" << micros_used << "micros)" << endl;

	//print results
	
	float sum = 0;
	for (int i=0; i<pr.size(); i++){
		sum += pr[i];
	}cout << "SUM: " << sum << endl;

	return 0;
}
