#include "lector_txt.h"
#include <iostream>
#include <stdexcept>

using namespace Herramientas_proyecto;

Lector_txt::Lector_txt()
	:numero_linea(0), comentario('#')
{

}

Lector_txt::Lector_txt(const std::string& ruta, const char c)
	:numero_linea(0), comentario(c)
{
	abrir_archivo(ruta);
}

Lector_txt::~Lector_txt()
{
	if(archivo) archivo.close();
}

/**
* Lee la siguiente línea del archivo. Si determina que la siguiente
* línea es un comentario o está vacía intentará automáticamente leer la
* siguiente. Cuando no puede leer la siguiente devolverá una línea en
* blanco.
*/

std::string Lector_txt::leer_linea()
{
	if(archivo.eof())
	{
		linea_actual="";
	}
	else
	{
		++numero_linea;
		std::getline(archivo, linea_actual);
		if(!linea_actual.size() || (linea_actual.size() && linea_actual[0]==comentario))
		{
			return leer_linea();
		}
	}

	return linea_actual;
}

void Lector_txt::abrir_archivo(const std::string& ruta)
{
	if(archivo) archivo.close();
	numero_linea=0;
	linea_actual="";
	archivo.open(ruta.c_str());
}

std::vector<std::string> Herramientas_proyecto::obtener_entradas_lector_txt_desde_ruta(const std::string& ruta)
{
	Lector_txt L(ruta, '#');
	std::vector<std::string> resultado;

	if(!L)
	{
		throw std::runtime_error("Imposible obtener_entradas_lector_txt_desde_ruta en "+ruta);
	}

	while(true)
	{
		std::string linea=L.leer_linea();
		if(!L) break;
		resultado.push_back(linea);
	}

	return resultado;
}
