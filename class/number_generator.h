#ifndef TOOLS_NUMBER_GENERATOR_H
#define TOOLS_NUMBER_GENERATOR_H

#include <random>

namespace tools
{

#ifdef WINCOMPIL

class int_generator
{
	public:

//TODO: Do these behave the same???

	int_generator(int vmin, int vmax):vmin(vmin), vmax(vmax){}
	int operator()() 
	{
			int res=rand() % (vmax-vmin);
			return res+vmin;
	}

	private:

	int vmin, vmax;	
};

#else

class int_generator
{
	public:

	int_generator(int vmin, int vmax):dist{vmin, vmax}, re(std::random_device{}()) {}
	int operator()() {return dist(re);}

	private:

	std::uniform_int_distribution<> dist;
	std::default_random_engine re;
	
};

#endif

}
#endif
