#ifndef LISTADO_REJILLA_H
#define LISTADO_REJILLA_H

#include <iostream>
#include <vector>
#include "estructura_paginacion.h"

namespace Herramientas_proyecto
{

template<typename T>
class Listado_rejilla
{
	public:


	//Estructura que se sirve como parte de un listado.
	struct Item
	{
		size_t x, y, indice;
		const T& item;
	};

						//Propios...
						Listado_rejilla(size_t w_disponible, size_t h_disponible, size_t w_item, size_t h_item)
		:w_disponible(w_disponible), h_disponible(h_disponible),
		w_item(w_item), h_item(h_item), 
		reg_fila(floor(w_disponible / w_item)),
		reg_columna(floor(h_disponible / h_item))
	{
		estructura_paginacion.establecer_registros_por_pagina(reg_fila * reg_columna);
	}

	void				size() const 
	{
		return lineas.size();
	}

	const T&			operator[](size_t v) const 
	{
		return lineas[v];
	}

	const T&			item_actual() const 
	{
		return lineas[estructura_paginacion.acc_indice_actual()];
	}

	const Item			linea_actual() const 
	{
		size_t indice=estructura_paginacion.acc_indice_actual();
		size_t rpp=estructura_paginacion.acc_registros_por_pagina();
		size_t rel=indice % rpp;

		size_t y=floor(rel / reg_fila);
		size_t x=floor(rel % reg_fila);

		return Item{x * w_item, y * h_item, indice, item_actual()};
	}

	void 					insertar(const T& v)
	{
		lineas.push_back(v);
		estructura_paginacion.establecer_total_elementos(estructura_paginacion.acc_total_elementos()+1);
	}

	std::vector<Item>			obtener_pagina() const
	{
		std::vector<Item> res;

		size_t rpp=estructura_paginacion.acc_registros_por_pagina();
		size_t pa=estructura_paginacion.acc_pagina_actual();

		auto	ini=std::begin(lineas)+(pa * rpp),
			fin=ini+rpp;

		//TODO...
		size_t x=0, y=0, reg=1, indice=pa * rpp;

		while(ini < fin && ini < std::end(lineas))
		{
			res.push_back(Item{x, y, indice++, *ini});
			++ini;

			if(reg == reg_fila)
			{
				x=0;
				reg=1;
				y+=h_item;
			}
			else
			{
				++reg;
				x+=w_item;
			}
		}

		return res;
	}

	void					clear()
	{
		lineas.clear();
		estructura_paginacion.establecer_total_elementos(0);
	}

	template<typename E>
	bool 					selector_topologico(size_t rx, size_t ry, E f)
	{
		const auto& pag=obtener_pagina();
		for(const auto& itemp : pag)
		{
			if(	rx >= itemp.x
				&& ry >= itemp.y
				&& rx <= itemp.x + w_item
				&& ry <= itemp.y + h_item)
			{
				f(itemp);
				return true;
			}
		}
		return false;
	}	

	//Passthroughs paginación...
	bool					cambiar_pagina(int v) 	{return estructura_paginacion.cambiar_pagina(v);}
	bool					cambiar_item(int v)	{return estructura_paginacion.cambiar_item(v);}
	bool					cambiar_fila(int v)
	{
		bool resultado=false;
		for(size_t i=0; i < reg_fila ; ++i) resultado=estructura_paginacion.cambiar_item(v) || resultado;
		return resultado;
	}
	void					reiniciar_indice() 	{estructura_paginacion.reiniciar_indice();}
	void					mut_indice(size_t v)	{estructura_paginacion.mut_indice(v);}
	size_t					acc_pagina_actual() const {return estructura_paginacion.acc_pagina_actual();}
	size_t					acc_indice_actual() const {return estructura_paginacion.acc_indice_actual();}
	size_t					acc_total_paginas() const {return estructura_paginacion.acc_total_paginas();}
	size_t					acc_registros_por_pagina() const {return estructura_paginacion.acc_registros_por_pagina();}
	size_t					acc_w_item() const {return w_item;}
	size_t					acc_h_item() const {return h_item;}

	protected:

	Estructura_paginacion			estructura_paginacion;
	std::vector<T>				lineas;

	size_t					w_disponible, h_disponible;
	size_t					w_item, h_item;
	size_t					reg_fila, reg_columna;
};

}
#endif
