#ifndef TEMPLATE_MATRIZ2D
#define TEMPLATE_MATRIZ2D

#include <map>
#include <stdexcept>

/*Una matriz 2D implementada en términos de un std::map. Se crea a partir de
unas dimensiones que le damos y contiene sólo los elementos que le damos.
En caso de intentar acceder a un elemento inexistente se lanzará una
excepción.
Sólo sirve para rangos positivos. Una implementación extendida (con cuatro mapas
internos) serviría para representar una matriz que se extiende tanto en
positivo como en negativo.
Requisitos:
- Sólo sirve para rangos positivos en x e y.
- Es obligatorio especificar ancho y alto.
- El elemento debe ser copiable.
Viene acompañado de una estructura Item_coordenadas, que la podemos usar para
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
- Para comprobar rápidamente si hay algo en unas coordenadas... Ojo, puede lanzar una excepción si nos vamos fuera de los límites.
	bool count(unsigned int x, unsigned int y)
- Comprobar, con seguridad de excepciones...
	bool comprobar(unsigned int x, unsigned int y)
-Una forma de acceder a los mapas subyacentes.
	const std::map<unsigned int, T>& acc_matriz() const;
	std::map<unsigned int, T>& acc_matriz();
-Obtener el total de elementos existentes.
	size_t size() const;
Los tipos de excepción en uso:
	Matriz_2d_excepcion : cualquier cosa que falle.
	Matriz_2d_excepcion_fuera_limites : se indican x o y que quedan fuera del rango.
	Matriz_2d_excepcion_item_invalido : se intenta obtener un valor en coordenadas donde no hay nada.
	Matriz_2d_excepcion_item_existe	 : se intenta insertar un valor en coordenadas donde ya hay algo.
*/

namespace Herramientas_proyecto
{

struct Matriz_2d_excepcion:public std::logic_error
{
	int x, y;
	Matriz_2d_excepcion(int px, int py):logic_error("Error en matriz2d"), x(px), y(py) {}
};

struct Matriz_2d_excepcion_fuera_limites:public Matriz_2d_excepcion 
{
	Matriz_2d_excepcion_fuera_limites(int px, int py):Matriz_2d_excepcion(px, py) {}
};

struct Matriz_2d_excepcion_item_invalido:public Matriz_2d_excepcion 
{
	Matriz_2d_excepcion_item_invalido(int px, int py):Matriz_2d_excepcion(px, py) {}
};

struct Matriz_2d_excepcion_item_existe:public Matriz_2d_excepcion 
{
	Matriz_2d_excepcion_item_existe(int px, int py):Matriz_2d_excepcion(px, py) {}
};

template<typename T>
struct Item_coordenadas
{
	unsigned int x, y;
	T elem;

	Item_coordenadas(unsigned int px, unsigned int py, T e): x(px), y(py), elem(e) 
	{}

	Item_coordenadas(unsigned int px, unsigned int py, T&& e): x(px), y(py), elem(std::move(e))
	{}

	//TODO: Estos estarían aquí para operaciones futuras, como redimensionar y cosas así.
//		Item(const Item& o):x(o.x), y(o.y), elem(e.elem) {}
//		Item(const Item&& o):x(o.x), y(o.y), elem(std::move(e.elem)) {}
};

template<typename T>
class Matriz_2d
{
	///////////////////////////
	// Definiciones.
	public:

	typedef std::pair<unsigned int, T> par;

	///////////////////////////
	// Propiedades.
	private:

	struct Coords
	{
		unsigned int x;
		unsigned int y;
		Coords(unsigned int px, unsigned int py): x(px), y(py) {}
	};

	std::map<unsigned int, T> matriz;
	unsigned int w, h;

	///////////////////////////
	// Métodos internos.
	private:

	unsigned int coordenadas_a_indice(unsigned int x, unsigned int y) const
	{
		if(x >= w || y >= h) throw Matriz_2d_excepcion_fuera_limites(x, y);
		return (y * w) + x;
	}

	Coords indice_a_coordenadas(unsigned int indice) const
	{
		int y=indice / w;
		int x=indice % w;
		return Coords(x, y);
	}

	//////////////////////////
	// Interfaz pública.
	public:

	Matriz_2d(unsigned int pw, unsigned int ph):
	w(pw), h(ph)
	{}

	Matriz_2d(const Matriz_2d& o):
		matriz(o.matriz), w(o.w), h(o.h)
	{}

	void insert(unsigned int x, unsigned int y, T& val)
	{
		unsigned int indice=coordenadas_a_indice(x, y);
		if(matriz.count(indice)) throw Matriz_2d_excepcion_item_existe(x, y);
		matriz.insert(std::make_pair(indice, val));
	}

	//TODO: Leer un poco sobre esto, a ver si merece la pena o no.
	void insert(unsigned int x, unsigned int y, T&& val)
	{
		unsigned int indice=coordenadas_a_indice(x, y);
		if(matriz.count(indice)) throw Matriz_2d_excepcion_item_existe(x, y);
		matriz.insert(std::make_pair(indice, val));
	}

	void erase(unsigned int x, unsigned int y)
	{
		unsigned int indice=coordenadas_a_indice(x, y);
		if(!matriz.count(indice)) throw Matriz_2d_excepcion_item_invalido(x, y);
		matriz.erase(indice);
	}

	//Se usa "at" para evitar la necesidad de constructores por defecto.
	const T& operator()(unsigned int x, unsigned int y) const
	{
		unsigned int indice=coordenadas_a_indice(x, y);
		if(!matriz.count(indice)) throw Matriz_2d_excepcion_item_invalido(x, y);
		return matriz.at(indice);
	}

	T& operator()(unsigned int x, unsigned int y)
	{
		unsigned int indice=coordenadas_a_indice(x, y);
		if(!matriz.count(indice)) throw Matriz_2d_excepcion_item_invalido(x, y);
		return matriz.at(indice);
	}

	//Permite inserciones...
	T& operator()(unsigned int x, unsigned int y, T& val)
	{
		unsigned int indice=coordenadas_a_indice(x, y);
		if(matriz.count(indice)) throw Matriz_2d_excepcion_item_existe(x, y);
		matriz.insert(std::make_pair(indice, val));
		return matriz.at(indice);
	}

	T& operator()(unsigned int x, unsigned int y, T&& val)
	{
		unsigned int indice=coordenadas_a_indice(x, y);
		if(matriz.count(indice)) throw Matriz_2d_excepcion_item_existe(x, y);
		matriz.insert(std::make_pair(indice, val));
		return matriz.at(indice);
	}

	//Para comprobar rápidamente si hay algo en unas coordenadas... Ojo, puede lanzar una excepción si nos vamos fuera de los límites.
	bool count(unsigned int x, unsigned int y) const {return matriz.count(coordenadas_a_indice(x, y));}

	bool comprobar(unsigned int x, unsigned int y) const
	{
		if(x >= w || y >= h) return false;
		else 
		{
			unsigned int indice=coordenadas_a_indice(x, y);
			return matriz.count(indice);
		}
	}

	//Una forma de acceder a los mapas subyacentes.
	const std::map<unsigned int, T>& acc_matriz() const {return matriz;}
	std::map<unsigned int, T>& acc_matriz() {return matriz;}

	size_t size() const {return matriz.size();}

	Matriz_2d<T> copiar_con_dimensiones(unsigned int pw, unsigned int ph) const
	{
		Matriz_2d<T> resultado(pw, ph);	

		for(const std::pair<unsigned int, T>& p : matriz)
		{
			auto c=indice_a_coordenadas(p.first);
			auto copia=p.second;
			if(c.x < resultado.w && c.y < resultado.h) resultado(c.x, c.y, copia);
		}
		return resultado;
	}

	template <typename TipoFunc> void aplicar(TipoFunc& f) const
	{
		for(auto& p : matriz) f(p.second);
	}
};

}
#endif
