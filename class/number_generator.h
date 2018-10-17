#ifndef TOOLS_NUMBER_GENERATOR_H
#define TOOLS_NUMBER_GENERATOR_H

#include <random>

namespace tools
{

#ifdef WINCOMPIL

//!Generator of integer number for mingw. 
class int_generator {
	public:

//TODO: Do these behave the same???

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

	private:

	int 		vmin,		//!Minimal value (inclusive).
			vmax;		//!Maximum value (inclusive).
};

#else

//!Random integer generator with uniform distribution.
class int_generator {
	public:

	//!Constructs the generator with the min and max values (inclusive).
			int_generator(int vmin, int vmax):dist{vmin, vmax}, re(std::random_device{}()) {
	}

	//!Gets the next number in the sequence.
	int operator()() {
		return dist(re);
	}

	private:

	std::uniform_int_distribution<> dist;	//!< Distribution method.
	std::default_random_engine re;		//!< Random number engine.
	
};

#endif

}
#endif
