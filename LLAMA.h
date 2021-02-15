#include <stdint.h>
#include <string>
#include <vector>
#include "vertex.h"

using namespace std;

typedef vertex* page; //arr. of vertices
typedef page* i_table;
typedef int* e_table; //adj. list


class LLAMA {

	private:
		bool in_llama;

		int cur_sid; //current snapshot
		int target_sid; //target snapshot
		int max_sid;   //max snspshot id

		int m;
		int num_pages;
		int page_size;

		//vetex table
		i_table* i_tables;
		//edge table
		e_table* e_tables;

		//used for initializing llama
		vector<int>* tmp;

		void add(int from, int to);
		int create_snapshot();
		pair<uint32_t, uint32_t> convert(int vid);

	public:
		LLAMA();
		LLAMA(int max_id, int target_, int m_, int p_, int v_, int e_, bool in);

		int vertices, edges;
		int* deg;

		void read(string fname);
		void print_(int sid);

		int load(int edges_in_snapshot);

		vector<int> neighbors(int vid);
	
};
