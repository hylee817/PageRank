#include <vector>
using namespace std;


class INCSR {
	private:
		vector<int> *tmp;
		int *out;
		int cur;
		
		int v_ptr;
		int e_ptr;

		int *idx;
		int *val;
	public:
		int vertices;
		int edges;

		INCSR();

		void set(int v, int e);
		void add(int from, int to);
		void end();
		void print_state();
		void print_idx();
		void print_val();
		
		int base(int to);
		int at(int seq);
		int at(int to, int seq);
		int in_deg(int to);
		int out_deg(int from);
		int get_v();
		int get_e();
};
