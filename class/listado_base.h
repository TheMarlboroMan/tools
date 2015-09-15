#ifndef HERRAMIENTAS_PROYECTO_LISTADO_BASE_H
#define HERRAMIENTAS_PROYECTO_LISTADO_BASE_H

#include <iostream>
#include <vector>
#include "estructura_paginacion.h"

namespace Herramientas_proyecto
{

template<typename T>
class Listado_base
{
	public:

						Listado_base(size_t hd, size_t hi)
		:h_disponible(hd), h_item(hi)
	{

	}

	void					clear()
	{
		lineas.clear();
		estructura_paginacion.establecer_total_elementos(0);
	}

	void 					insertar(const T& v)
	{
		lineas.push_back(v);
		estructura_paginacion.establecer_total_elementos(estructura_paginacion.acc_total_elementos()+1);
	}

	void					size() const 
	{
		return lineas.size();
	}

	const T&				operator[](size_t v) const 
	{
		return lineas[v];
	}


	const T&				item_actual() const 
	{
		return lineas[estructura_paginacion.acc_indice_actual()];
	}

	//Passthroughs paginación...
	void					reiniciar_indice() 	{estructura_paginacion.reiniciar_indice();}
	void					mut_indice(size_t v)	{estructura_paginacion.mut_indice(v);}
	bool					cambiar_pagina(int v) 	{return estructura_paginacion.cambiar_pagina(v);}
	bool					cambiar_item(int v)	{return estructura_paginacion.cambiar_item(v);}
	size_t					acc_pagina_actual() const {return estructura_paginacion.acc_pagina_actual();}
	size_t					acc_indice_actual() const {return estructura_paginacion.acc_indice_actual();}
	size_t					acc_total_paginas() const {return estructura_paginacion.acc_total_paginas();}
	size_t					acc_registros_por_pagina() const {return estructura_paginacion.acc_registros_por_pagina();}
	size_t					acc_h_item() const	{return h_item;}

	protected:

	std::vector<T>				lineas;
	Estructura_paginacion			estructura_paginacion;

	size_t					h_disponible;
	size_t					h_item;
};
}

#endif
