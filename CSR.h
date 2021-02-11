#include <vector>
using namespace std;


class CSR {
	private:

		int v_ptr; //
		int e_ptr; //

		int *idx;
		int *val;
	public:
		int vertices;
		int edges;

		CSR();

		void set(int v, int e);
		void add(int from, int to);
		void end();
		void print_state();
		void print_idx();
		void print_val();
		
		int at(int from, int seq);
		int out_deg(int from);
		int get_v();
		int get_e();
};
