#include "pair_file_parser.h"
#include <fstream>


using namespace tools;

pair_file_parser::pair_file_parser(const std::string& nf, char sep, char com):
	filename(nf), delimiter(sep), comment(com)
{
	load();
}

/**
* Carga el contenido del fichero en el mapa.
*/

void pair_file_parser::load()
{
	try
	{
		data=map_pair(filename, delimiter, comment);
	}
	catch(std::exception& e)
	{
		throw std::runtime_error("Unable to parse pair file "+filename+" : "+e.what());
	}
}

/**
* Guarda el contenido del mapa en el fichero. Al usarse un mapa, que es un contenedor
* si ordenar, no se conservará el orden original de los elementos. De la misma forma
* no se conservarán comments o líneas en blanco que estuvieran presentes en
* el fichero original.
*/
 
void pair_file_parser::save()
{
	std::ofstream fichero(filename.c_str());
	
	for(const auto& par : data)
	{
		fichero<<par.first<<delimiter<<par.second<<std::endl;
	}
}

void pair_file_parser::sync(const pair_file_parser& f)
{
	for(const auto& par : f.data)
	{
		if(!data.count(par.first)) 
			data[par.first]=f.data.at(par.first);
	}
}
