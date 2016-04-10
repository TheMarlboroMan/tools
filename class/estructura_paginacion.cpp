#include "estructura_paginacion.h"

#include <iostream>

using namespace Herramientas_proyecto;

/**
* @param size_t alt : altura de la línea.
* @param size_t t : total de elementos
* @param size_t h : altura del espacio reservado para el listado.
*/

Estructura_paginacion::Estructura_paginacion(size_t rpp, size_t total_elementos)
	:registros_por_pagina(rpp), total_elementos(total_elementos), pagina_actual(0),
	total_paginas(0), indice_actual(0)
{
	//Prevenir una división por 0...
	if(registros_por_pagina && total_elementos)
	{
		calcular_informacion_paginas();
	}
}

void Estructura_paginacion::establecer_total_elementos(size_t val)
{
	total_elementos=val;
	calcular_informacion_paginas();
}

void Estructura_paginacion::establecer_registros_por_pagina(size_t val)
{
	registros_por_pagina=val;
	calcular_informacion_paginas();
}

void Estructura_paginacion::calcular_informacion_paginas()
{
	if(registros_por_pagina) total_paginas=ceil(total_elementos / registros_por_pagina);
}

bool Estructura_paginacion::cambiar_pagina(int val, bool forzar)
{
	if(val > 0)
	{
		if(pagina_actual < total_paginas)
		{
			++pagina_actual;
			if(forzar) indice_actual+=registros_por_pagina;
		}
		else if(pagina_actual == total_paginas)
		{
			indice_actual=total_elementos-1;
		}

		return true;
	}
	else if(val < 0)
	{
		if(pagina_actual > 0) 
		{
			--pagina_actual;
			if(forzar) indice_actual-=registros_por_pagina;
		}
		else
		{
			indice_actual=0;
		}

		return true;
	}
	else return false;
}

bool Estructura_paginacion::cambiar_item(int val)
{
	bool resultado=false;

	if(val > 0 && indice_actual + 1 < total_elementos)
	{
		++indice_actual;
		resultado=true;
	}
	else if(val < 0 && indice_actual > 0)
	{
		--indice_actual;
		resultado=true;
	}
	
	if(resultado)
	{
		//Detectar si vamos a cambiar de página... Se hace en dos direcciones...
		size_t indice=indice_actual % (registros_por_pagina);
		if(val > 0)
		{
			if(!indice)
			{
				cambiar_pagina(val, false);
			}
		}
		else if(val < 0)
		{
			if(indice==registros_por_pagina - 1)
			{
				cambiar_pagina(val, false);
			}
	
		}
	}

	return resultado;
}
