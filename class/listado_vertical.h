#ifndef LISTADO_VERTICAL_H
#define LISTADO_VERTICAL_H

#include <iostream>
#include <vector>
#include "estructura_paginacion.h"

namespace Herramientas_proyecto
{

template<typename T>
class Listado_vertical
{
	public:


	//Estructura que se sirve como parte de un listado.
	struct Item
	{
		size_t y, indice;
		const T& item;
	};

						//Propios...
						Listado_vertical(size_t altura_disponible, size_t h_item)
		:altura_disponible(altura_disponible), h_item(h_item)
	{
		estructura_paginacion.establecer_registros_por_pagina(floor(altura_disponible / h_item));
	}

	void					size() const 
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

	const T&			item(size_t i) const 
	{
		return lineas[i];
	}

	const Item			linea_actual() const 
	{
		return Item{ h_item * ( estructura_paginacion.acc_indice_actual() % estructura_paginacion.acc_registros_por_pagina()),  
			estructura_paginacion.acc_indice_actual(),
			item_actual()
		};
	}

	const Item			linea(size_t i) const 
	{
		return Item{h_item * i, estructura_paginacion.acc_indice_actual(), item(i)};
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

		size_t y=0, indice=pa*rpp;

		while(ini < fin && ini < std::end(lineas))
		{
			res.push_back(Item{y, indice++, *ini});
			++ini;
			y+=h_item;
		}

		return res;
	}

	void					clear()
	{
		lineas.clear();
		estructura_paginacion.establecer_total_elementos(0);
	}

	template<typename E> 
	bool 					selector_topologico(size_t ry, E f)
	{
		const auto& pag=obtener_pagina();
		for(const auto& itemp : pag)
		{			
			if(ry >= itemp.y && ry <= itemp.y + h_item)
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
	void					reiniciar_indice() 	{estructura_paginacion.reiniciar_indice();}
	void					mut_indice(size_t v)	{estructura_paginacion.mut_indice(v);}
	size_t					acc_pagina_actual() const {return estructura_paginacion.acc_pagina_actual();}
	size_t					acc_indice_actual() const {return estructura_paginacion.acc_indice_actual();}
	size_t					acc_total_paginas() const {return estructura_paginacion.acc_total_paginas();}
	size_t					acc_registros_por_pagina() const {return estructura_paginacion.acc_registros_por_pagina();}
	size_t					acc_h_item() const	{return h_item;}

	protected:

	Estructura_paginacion			estructura_paginacion;
	std::vector<T>				lineas;

	size_t					altura_disponible;
	size_t					h_item;
};

}
#endif
