#ifndef HERRAMIENTAS_PROYECTO_LISTADO_REJILLA_H
#define HERRAMIENTAS_PROYECTO_LISTADO_REJILLA_H

#include "listado_base.h"

namespace Herramientas_proyecto
{

template<typename T>
class Listado_rejilla:public Listado_base<T>
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
		:Listado_base<T>(h_disponible, h_item),
		w_disponible(w_disponible), w_item(w_item), 
		reg_fila(floor(w_disponible / w_item)),
		reg_columna(floor(h_disponible / h_item))
	{
		this->estructura_paginacion.establecer_registros_por_pagina(reg_fila * reg_columna);
	}


	const Item				linea_actual() const 
	{
		size_t indice=this->estructura_paginacion.acc_indice_actual();
		size_t rpp=this->estructura_paginacion.acc_registros_por_pagina();
		size_t rel=indice % rpp;

		size_t y=floor(rel / reg_fila);
		size_t x=floor(rel % reg_fila);

		return Item{x * w_item, y * this->h_item, indice, this->item_actual()};
	}


	std::vector<Item>			obtener_pagina() const
	{
		std::vector<Item> res;

		size_t rpp=this->estructura_paginacion.acc_registros_por_pagina();
		size_t pa=this->estructura_paginacion.acc_pagina_actual();

		auto	ini=std::begin(this->lineas)+(pa * rpp),
			fin=ini+rpp;

		size_t x=0, y=0, reg=1, indice=pa * rpp;

		while(ini < fin && ini < std::end(this->lineas))
		{
			res.push_back(Item{x, y, indice++, *ini});
			++ini;

			if(reg == reg_fila)
			{
				x=0;
				reg=1;
				y+=this->h_item;
			}
			else
			{
				++reg;
				x+=w_item;
			}
		}

		return res;
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
				&& ry <= itemp.y + this->h_item)
			{
				f(itemp);
				return true;
			}
		}
		return false;
	}	

	//Especializaciones paginación...
	bool					cambiar_fila(int v)
	{
		bool resultado=false;
		for(size_t i=0; i < reg_fila ; ++i) resultado=this->estructura_paginacion.cambiar_item(v) || resultado;
		return resultado;
	}
	size_t					acc_w_item() const {return w_item;}

	private:

	size_t					w_disponible;
	size_t					w_item;
	size_t					reg_fila, reg_columna;
};

}
#endif
