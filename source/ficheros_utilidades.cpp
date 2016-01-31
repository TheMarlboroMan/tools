#include "ficheros_utilidades.h"

using namespace Herramientas_proyecto;

bool Herramientas_proyecto::existe_fichero(const std::string& c)
{
	std::ifstream f(c);
	return f.is_open();
}

std::string Herramientas_proyecto::volcar_fichero(const std::string& c)
{
	std::string res, lin;
	std::ifstream f(c);
	
	while(true)
	{
		if(f.eof()) break;
		std::getline(f, lin);
		res+=lin+"\n";
	}

	return res;
}
