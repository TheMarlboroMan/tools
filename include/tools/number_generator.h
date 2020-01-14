#pragma once

#include <random>

namespace tools
{

//!Random integer generator with uniform distribution.
class int_generator {
	public:

#ifdef WINCOMPIL
	//!Constructs the generator with the min and max values (inclusive).
			int_generator(int vmin, int vmax):vmin(vmin), vmax(vmax){
	}
	
	//!Gets the next number in the sequence.
	int 		operator()() {
		if(vmax==vmin) {
			return vmax;
		}
		int res=rand() % (vmax-vmin);
		return res+vmin;
	}
#else
	//!Constructs the generator with the min and max values (inclusive).
			int_generator(int vmin, int vmax):dist{vmin, vmax}, re(std::random_device{}()) {
	}

	//!Gets the next number in the sequence.
	int operator()() {
		return dist(re);
	}
#endif
	private:

#ifdef WINCOMPIL
	int 		vmin,		//!Minimal value (inclusive).
			vmax;		//!Maximum value (inclusive).
#else
	std::uniform_int_distribution<> dist;	//!< Distribution method.
	std::default_random_engine re;		//!< Random number engine.
#endif
};

}
