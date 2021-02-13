#include <iostream>
#include <fstream>
#include <stdexcept>
#include <cstring>
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
//const int snapshots = 10;

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

//LLAMA llama(snapshots, 10, 4800, true);
//LLAMA llama(snapshots, 5, 128, true);

vector<float> compute_PR(LLAMA llama){
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



int main(int argc, char* argv[]){

	string _fname;
	int _m, _p, _v, _e, _max, _id;
	bool _in;

	/*
	for (int i=0; i<argc; i++){
		if(!strcmp(argv[i], "-h") || !strcmp(argv[i], "-help")){
			printf("Usage: App <options>\nOptions are:\n");
			printf("-lj (soc-livejournals1.txt)\n-fb (FaceBook_combined.txt)\n-in (page rank done in incoming edges)\n");
			exit(0);
		}else if(!strcmp(argv[i], "-lj")){
			_fname = "../soc-LiveJournal1.txt";
			_m = 10;
			_p = 4800;
			_v = 4847571;
			_e = 68993773;
		}else if(!strcmp(argv[i], "-fb")){
			_fname = "../facebook_combined.txt";
			_m = 5;
			_p = 128;
			_v = 4039;
			_e = 88234;
		}else if(!strcmp(argv[i], "-in")){
			_in = true;
		}else{
			if(i == argc-1){break;}
			printf("ERROR: Invalid Command Line Option Found");
			exit(1);
		}
	}
	*/

	cout << "fb || lj: ";
	cin >> _fname;
	if (!_fname.compare("fb")){_m=5;_p=128;_v=4039;_e=88234;_fname="../facebook_combined.txt";}
	else if(!_fname.compare("lj")){_m=10;_p=4800;_v=4847571;_e=68993773;_fname="../soc-LiveJournal1.txt";}
	else{cout << "Wrong Argument"; exit(1);}
	cout << "max snapshot: ";
	cin >> _max;
	cout << "snapshot to run page rank on: ";
	cin >> _id;

	LLAMA llama(_max, _m, _p, _v, _e, _in);
	llama.read(_fname);
	llama.set(_id);

	struct timeval start,end;
	long secs_used, micros_used;

	gettimeofday(&start, NULL);
	vector<float> pr = compute_PR(llama);
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
