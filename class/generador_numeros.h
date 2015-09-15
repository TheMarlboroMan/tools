#ifndef GENERADOR_NUMEROS_H
#define GENERADOR_NUMEROS_H

#include <random>

namespace HerramientasProyecto
{

class Generador_int
{
	public:

	Generador_int(int vmin, int vmax):dist{vmin, vmax}, re(std::random_device{}()) {}
	int operator()() {return dist(re);}

	private:

	std::uniform_int_distribution<> dist;
	std::default_random_engine re;
	
};

}
#endif
