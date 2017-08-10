#ifndef TOOLS_MATRIX_2D_H
#define TOOLS_MATRIX_2D_H

#include <map>
#include <stdexcept>

/*Una data 2D implementada en términos de un std::map. Se crea a t_pairtir de
unas dimensiones que le damos y contiene sólo los elementos que le damos.
En caso de intentar acceder a un elemento inexistente se lanzará una
excepción.
Sólo sirve t_paira rangos positivos. Una implementación extendida (con cuatro mapas
internos) serviría t_paira representar una data que se extiende tanto en
positivo como en negativo.
Requisitos:
- Sólo sirve t_paira rangos positivos en x e y.
- Es obligatorio especificar ancho y alto.
- El elemento debe ser copiable.
Viene acompañado de una estructura matrix_2d_item, que la podemos usar t_paira
guardar elementos y conservar la información de la posición X e Y de las mismas.
Los métodos:
- Para insertar un item.
	void insert(unsigned int x, unsigned int y, T& val)
	void insert(unsigned int x, unsigned int y, T&& val)
- Para recuperar un item...
	const T& operator()(unsigned int x, unsigned int y) const
	T& operator()(unsigned int x, unsigned int y)
- Para insertar un item y recuperarlo...
	T& operator()(unsigned int x, unsigned int y, T& val)
	T& operator()(unsigned int x, unsigned int y, T&& val)
- Para check rápidamente si hay algo en unas coordenadas... Ojo, puede lanzar una excepción si nos vamos fuera de los límites.
	bool count(unsigned int x, unsigned int y)
- Comprobar, con seguridad de excepciones...
	bool check(unsigned int x, unsigned int y)
-Una forma de acceder a los mapas subyacentes.
	const std::map<unsigned int, T>& get_data() const;
	std::map<unsigned int, T>& get_data();
-Obtener el total de elementos existentes.
	size_t size() const;
Los tipos de excepción en uso:
	matrix_2d_exception : cualquier cosa que falle.
	matrix_2d_exception_bounds : se indican x o y que quedan fuera del rango.
	matrix_2d_exception_missing : se intenta obtener un valor en coordenadas donde no hay nada.
	matrix_2d_exception_conflict	 : se intenta insertar un valor en coordenadas donde ya hay algo.
*/

namespace tools
{

struct matrix_2d_exception:
	public std::runtime_error
{
	int 			x, 
				y;
	matrix_2d_exception(int px, int py):
		std::runtime_error("matrix_2d error"), x(px), y(py) 
	{
	}
};

struct matrix_2d_exception_bounds:
	public matrix_2d_exception 
{
	matrix_2d_exception_bounds(int px, int py)
		:matrix_2d_exception(px, py) {}
};

struct matrix_2d_exception_missing:
	public matrix_2d_exception 
{
	matrix_2d_exception_missing(int px, int py)
		:matrix_2d_exception(px, py) {}
};

struct matrix_2d_exception_conflict:
	public matrix_2d_exception 
{
	matrix_2d_exception_conflict(int px, int py)
		:matrix_2d_exception(px, py) {}
};

template<typename T>
struct matrix_2d_item
{
	unsigned int x, y;
	T elem;

	matrix_2d_item(unsigned int px, unsigned int py, T e): x(px), y(py), elem(e) 
	{}

	matrix_2d_item(unsigned int px, unsigned int py, T&& e): x(px), y(py), elem(std::move(e))
	{}

	//TODO: Estos estarían aquí para operaciones futuras, como redimensionar y cosas así.
//		Item(const Item& o):x(o.x), y(o.y), elem(e.elem) {}
//		Item(const Item&& o):x(o.x), y(o.y), elem(std::move(e.elem)) {}
};

template<typename T>
class matrix_2d
{
	public:

	typedef std::pair<unsigned int, T> t_pair;

					matrix_2d(unsigned int pw, unsigned int ph)
		:w(pw), h(ph)
	{

	}

					matrix_2d(const matrix_2d& o)
		:data(o.data), w(o.w), h(o.h)
	{

	}

	void 				insert(unsigned int x, unsigned int y, T& val)
	{
		unsigned int index=coords_to_index(x, y);
		if(data.count(index)) throw matrix_2d_exception_conflict(x, y);
		data.insert(std::make_pair(index, val));
	}

	void 				insert(unsigned int x, unsigned int y, T&& val)
	{
		unsigned int index=coords_to_index(x, y);
		if(data.count(index)) throw matrix_2d_exception_conflict(x, y);
		data.insert(std::make_pair(index, val));
	}

	void 				erase(unsigned int x, unsigned int y)
	{
		unsigned int index=coords_to_index(x, y);
		if(!data.count(index)) throw matrix_2d_exception_missing(x, y);
		data.erase(index);
	}

	//Se usa "at" para evitar la necesidad de constructores por defecto.
	const T& 			operator()(unsigned int x, unsigned int y) const
	{
		unsigned int index=coords_to_index(x, y);
		if(!data.count(index)) throw matrix_2d_exception_missing(x, y);
		return data.at(index);
	}

	T& 				operator()(unsigned int x, unsigned int y)
	{
		unsigned int index=coords_to_index(x, y);
		if(!data.count(index)) throw matrix_2d_exception_missing(x, y);
		return data.at(index);
	}

	//Permite inserciones...
	T& 				operator()(unsigned int x, unsigned int y, T& val)
	{
		unsigned int index=coords_to_index(x, y);
		if(data.count(index)) throw matrix_2d_exception_conflict(x, y);
		data.insert(std::make_pair(index, val));
		return data.at(index);
	}

	T& 				operator()(unsigned int x, unsigned int y, T&& val)
	{
		unsigned int index=coords_to_index(x, y);
		if(data.count(index)) throw matrix_2d_exception_conflict(x, y);
		data.insert(std::make_pair(index, val));
		return data.at(index);
	}

	//Para check rápidamente si hay algo en unas coordenadas... Ojo, puede lanzar una excepción si nos vamos fuera de los límites.
	bool 				count(unsigned int x, unsigned int y) const {return data.count(coords_to_index(x, y));}

	bool 				check(unsigned int x, unsigned int y) const
	{
		if(x >= w || y >= h) return false;
		else 
		{
			unsigned int index=coords_to_index(x, y);
			return data.count(index);
		}
	}

	size_t 				size() 	const {return data.size();}
	void				clear() {data.clear();}
	
	void				resize(unsigned int pw, unsigned int ph)
	{
		if(pw=w && ph==h) return;
	
		//We need to remember the old values to perform index_to_coords.
		unsigned int ow=w, oh=h;

		//Reassign...
		w=pw;
		h=ph;

		//This is the real final container.
		std::map<unsigned int, T> 	new_data;

		//Iterate and reinsert those whitin range.
		for(const auto& p : data)
		{
			auto c=index_to_coords(p.first, ow, oh);
			if(c.x < w && c.y < h)
			{
				new_data.insert(std::make_pair(coords_to_index(c.x, c.y), p.second));
			}
		}

		//Swap.
		std::swap(new_data, data);
	}

	matrix_2d<T> 			copy_and_resize(unsigned int pw, unsigned int ph) const
	{
		matrix_2d<T> result(pw, ph);

		for(const auto& p : data)
		{
			auto c=index_to_coords(p.first);
			auto copia=p.second;
			if(c.x < result.w && c.y < result.h) result(c.x, c.y, copia);
		}
		return result;
	}

	//Absolutely anything that has an operator () or even a function can be used with this :).
	template <typename TipoFunc> 
	void 				apply(TipoFunc& f) const
	{
		for(auto& p : data) f(p.second);
	}

	///////////////////////////
	// Propiedades.
	private:

	struct coords
	{
		unsigned int 		x,
					y;
		coords(unsigned int px, unsigned int py): x(px), y(py) {}
	};

	std::map<unsigned int, T> 	data;

	unsigned int 			w, 
					h;

	///////////////////////////
	// Métodos internos.
	private:

	unsigned int 			coords_to_index(unsigned int x, unsigned int y) const
	{
		if(x >= w || y >= h) throw matrix_2d_exception_bounds(x, y);
		return (y * w) + x;
	}

	coords 				index_to_coords(unsigned int index) const
	{
		int y=index / w;
		int x=index % w;
		return coords(x, y);
	}

	coords 				index_to_coords(unsigned int index, unsigned int pw, unsigned int ph) const
	{
		int y=index / pw;
		int x=index % pw;
		return coords(x, y);
	}
};

}
#endif
