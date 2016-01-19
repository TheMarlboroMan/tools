#include "fichero_pares.h"
#include <fstream>


//TODO: Delete.
#include <iostream>

using namespace Herramientas_proyecto;

Fichero_pares::Fichero_pares(const std::string& nf, char sep, char com):
	nombre_fichero(nf), separador(sep), comentario(com)
{
	cargar();
}

/**
* Carga el contenido del fichero en el mapa.
*/

void Fichero_pares::cargar()
{
	try
	{
		configuracion=generar_mapa_pares(nombre_fichero, separador, comentario);
	}
	catch(std::exception& e)
	{
		throw Fichero_pares_exception(nombre_fichero);
	}
}

/**
* Guarda el contenido del mapa en el fichero. Al usarse un mapa, que es un contenedor
* si ordenar, no se conservará el orden original de los elementos. De la misma forma
* no se conservarán comentarios o líneas en blanco que estuvieran presentes en
* el fichero original.
*/
 
void Fichero_pares::guardar()
{
	std::ofstream fichero(nombre_fichero.c_str());
	
	for(const auto& par : configuracion)
	{
		fichero<<par.first<<separador<<par.second<<std::endl;
	}
}

void Fichero_pares::sincronizar(const Fichero_pares& f)
{
	for(const auto& par : f.configuracion)
	{
		if(!configuracion.count(par.first)) 
			configuracion[par.first]=f.configuracion.at(par.first);
	}
}
