#include "string_utilidades.h"

using namespace Herramientas_proyecto;

std::vector<std::string> explotar(const std::string & p_cadena, const char p_delimitador, size_t max)
{
	size_t cuenta=0;

	std::vector<std::string> resultado;
	std::string::const_iterator ini=p_cadena.begin(), fin=p_cadena.end();
	std::string temp;

	while(ini < fin)
	{
		if( (*ini)==p_delimitador)
		{
			++cuenta;
			resultado.push_back(temp);
			temp="";
			if(max && cuenta >= max) 
			{	
				temp=std::string(ini+1, p_cadena.end());
				break;
			}
		}
		else
		{
			temp+=*ini;
		}
		
		++ini;
	}

	resultado.push_back(temp);
	return resultado;
}

std::map<std::string, std::string> generar_mapa_pares(const std::string& fichero, const char separador)
{
	std::map<std::string, std::string> res;
	return res;
}

size_t digitos_en_entero(int p_entero)
{
	if(p_entero==0) return 1;

	int copia=p_entero;
	int resultado=0;

	if(copia < 0) copia=-copia;
		
	while(copia > 0)
	{
		resultado++;
		copia/=10;
	}
	return resultado;
}
