#include <iostream>
#include <fstream>
#include <stdexcept>
#include <string>
#include <vector>
#include <math.h>
#include <stdio.h>
#include <sstream>
#define HAVE_STRUCT_TIMESPEC
#include <pthread.h>
#include "INCSR.h"
#include <sys/time.h>
#include <omp.h>
using namespace std;

const float eps = 1e-4;      //threshold
const float damp = 0.85;   //damping factor
const float beta = 0.8;      //random teleport

int NTHREADS;
bool is_mthread = false;
int M, N; //M=vtx, N=edges
INCSR csr;


void create_CSR(string fname){
	//temporary
	//M = 4039;
	//N = 88234;
	//csr.set(M,N);

	ifstream input(fname);
	if (input.fail()){
		cout << "Failed to open: " << fname << endl;
		exit(1);
	}

	string line;
	while(!input.eof()){
		getline(input, line);
		if (line.empty()) continue;
		if (line.find("Edges",0) != string::npos){
			M = stoi(line.substr(line.find(":")+1, line.find("Edges")));
			N = stoi(line.substr(line.find("Edges")+6));
			csr.set(M,N); //set size
			cout << M << " " << N << endl;
		}
		if (line.at(0) == '#') continue; //skip comments
		
		int from,to;
		stringstream ss(line);
		ss >> from; ss >> to;
		csr.add(from,to);	
	
	}input.close();
	csr.end();

}


vector<float> compute_PR(){
	float N = csr.vertices;
	vector<float> pr(N, 1/N);
	vector<float> out(N);
	float base = (1.0f - beta) / N;

	int count = 0;
	float diff = 1;
	while(diff > eps){
		count += 1;
		diff = 0;
		#pragma omp parallel for
		for (int id=0; id<csr.vertices;id++)
			out[id] = pr[id] / csr.out_deg(id);
	    #pragma omp parallel for if(is_mthread) reduction(+ : diff) schedule(dynamic, 64)
		for (int id=0;id<csr.vertices;id++){ //iterate vertices 
			float in_total = 0;
			int L = csr.in_deg(id);
			int pos = csr.base(id);
			for (int deg=0; deg<L; deg++){
				int from = csr.at(pos + deg);
				in_total += out[from];
			}
			float old = pr[id];
			pr[id] = base + beta*in_total;
			diff += fabs(pr[id] - old);
		}
		printf(" %2d    %lf\n", count, diff);
		//if (count ==20){break;}
	}

	return pr;
}


int main(){
	cout << "Number of threads to use: ";
	cin >> NTHREADS;
	if (NTHREADS > 0){is_mthread = true;}
	
	create_CSR("soc-LiveJournal1.txt");
	struct timeval start,end;
	long secs_used, micros_used;

	gettimeofday(&start, NULL);
	vector<float> pr = compute_PR();	
	gettimeofday(&end, NULL);

	secs_used = (end.tv_sec - start.tv_sec);
	micros_used = ((secs_used*1000000) + end.tv_usec) - (start.tv_usec);
	cout << "time(s): " << secs_used << "(" << micros_used << " micros)" << endl;

	//print results
	float sum = 0;
	for (int i=0; i<pr.size(); i++){
		//cout << i+1 << ": " << pr[i] << endl;
		sum += pr[i];
	}cout << "SUM: " << sum << endl;
}
