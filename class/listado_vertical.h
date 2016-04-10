#ifndef HERRAMIENTAS_PROYECTO_LISTADO_VERTICAL_H
#define HERRAMIENTAS_PROYECTO_LISTADO_VERTICAL_H

#include "listado_base.h"

namespace Herramientas_proyecto
{

template<typename T>
class Listado_vertical:public Listado_base<T>
{
	public:


	//Estructura que se sirve como parte de un listado.
	struct Item
	{
		size_t y, indice;
		const T& item;
	};

						//Propios...
						Listado_vertical(size_t h_disponible, size_t h_item)
		:h_disponible(h_disponible), h_item(h_item), margen_h(0)
	{
		this->estructura_paginacion.establecer_registros_por_pagina(floor(h_disponible / h_item));
	}

	const Item				linea_actual() const 
	{
		size_t i_actual=this->estructura_paginacion.acc_indice_actual();
		size_t indice=i_actual % this->estructura_paginacion.acc_registros_por_pagina();

		return Item{ 
			(h_item * indice) + (margen_h * indice),  
			i_actual,
			this->item_actual()
		};
	}

	const Item				linea(size_t i) const 
	{
		return Item{(h_item * i) + (margen_h * i), 
				this->estructura_paginacion.acc_indice_actual(), 
				this->lineas[i]};
	}

	std::vector<Item>			obtener_pagina() const
	{
		std::vector<Item> res;

		size_t rpp=this->estructura_paginacion.acc_registros_por_pagina();
		size_t pa=this->estructura_paginacion.acc_pagina_actual();

		auto	ini=std::begin(this->lineas)+(pa * rpp),
			fin=ini+rpp;

		size_t y=0, indice=pa*rpp;

		while(ini < fin && ini < std::end(this->lineas))
		{
			res.push_back(Item{y, indice++, *ini});
			++ini;
			y+=this->h_item+margen_h;
		}

		return res;
	}

	template<typename E> 
	bool 					selector_topologico(size_t ry, E f)
	{
		const auto& pag=obtener_pagina();
		for(const auto& itemp : pag)
		{			
			if(ry >= itemp.y && ry <= itemp.y + this->h_item)
			{
				f(itemp);
				return true;
			}
		}
		return false;
	}

	size_t					acc_h_item() const	{return h_item;}
	size_t					acc_margen_h() const	{return margen_h;}
	void					mut_margen_h(size_t v) {margen_h=v;}
	void					mut_h_disponible(size_t v) 
	{
		h_disponible=v;
		if(h_item) this->estructura_paginacion.establecer_registros_por_pagina(floor(h_disponible / h_item));
	}
	void					mut_h_item(size_t v) 
	{
		h_item=v;
		this->estructura_paginacion.establecer_registros_por_pagina(floor(h_disponible / h_item));
	}

	private:

	size_t					h_disponible;
	size_t					h_item;
	size_t					margen_h;
};

}
#endif
