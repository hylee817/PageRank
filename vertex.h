#include <stdint.h>
const int SID_BITS = 12;
const int LEN_BITS = 52;

class vertex{

	private:
		uint64_t sid_n_len_; //shared storage (12/52)
		uint64_t offset_;

	public: 
		vertex(){}
		void set(uint32_t sid, uint64_t len, uint64_t offset){
			sid_n_len_ = (((uint64_t)sid << LEN_BITS)| len);
			offset_ = offset;
		}
		uint32_t sid(){
			return sid_n_len_ >> LEN_BITS;
		}
		uint64_t len(){
			return sid_n_len_ & ((1 << SID_BITS) -1);
		}
		uint64_t offset(){
			return offset_;
		}

};
