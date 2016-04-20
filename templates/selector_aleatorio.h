#ifndef SELECTOR_ALEATORIO_H
#define SELECTOR_ALEATORIO_H

/**
* Template para obtener objetos "aleatorios" de un vector previamente 
* alimentado. A cada objeto insertado en el vector se le añade una probabilidad.
* Cuando se llama a obtener se escoge uno de acuerdo con la probabilidad y se
* devuelve.
* Las probabilidades hay que especificarlas a mano y siempre dentro del rango
* 1 - max (100 por defecto). Si la suma de todas las probabilidades es menor
* del máximo especificado hay posibilidad de que se tiren excepciones al 
* intentar obtener un valor.
* Abajo está el template uniforme, en que cada elemento tiene las mismas 
* posibilidades de salir. 
* Ambos comparten la misma base.
*/

#include "generador_numeros.h"
#include <vector>
#include <stdexcept>

//TODO: Quitar
#include <iostream>

namespace Herramientas_proyecto
{

class SelectorAleatorioException:public std::logic_error
{
	public:
	unsigned int val;
	SelectorAleatorioException(const std::string& c, unsigned int v)
		:logic_error(c), val(v)
	{}
};

template<typename T>
class Selector_aleatorio_variable
{
	////////////////////
	// Definiciones:
	private:

	struct Item
	{
		T t;
		unsigned int porcentaje;
	};

	////////////////////
	// Propiedades
	private:

	/* 
	* A cada item que se inserte se sumará la base. La base crece según el 
	* valor "porcentaje" para asegurarnos de que en el vector están 
	* ordenados por secciones. 
	*/

	unsigned int base;
	std::vector<Item> items;
	Generador_int G;

	////////////////////
	// Métodos privados.

	////////////////////
	// Interface pública
	public:

	Selector_aleatorio_variable(unsigned int max=100)
		:base(0), G(1, 100)
	{}

	void reset()
	{
		base=0;
		items.clear();
	}

	/**
	* @param const T& o
	* @param unsigned int p : como el porcentaje de posibilidades de que aparezca.
	*/

	void insertar(const T& o, unsigned int p)
	{
		items.push_back(Item {o, p+base});
		base+=p;
	}

	/**
	* @param unsigned int max 
	* Establece el máximo porcentaje posible para el generador de números. 
	* Por defecto es 100.
	*/

	void establecer_max(unsigned int max)
	{
		G=Generador_int(0, max);
	}

	/**
	* @throw SelectorAleatorioException
	* Obtiene un elemento aleatorio. Si el número escogido es mayor que
	* el porcentaje acumulado de items se lanzará una excepción.
	*/

	const T& obtener() const
	{
		//TODO: Debug...
		unsigned int val=G();

		std::cout<<val<<" ES VAL"<<std::endl;
		for(const T& t : items) std::cout<<t.porcentaje<<"% "<<std::endl;


		for(const T& t : items)
			if(t.porcentaje <= val) return t.t;

		throw SelectorAleatorioException("No se encuentra valor en SelectorAleatorio", val);
	}
};

template<typename T>
class Selector_aleatorio_uniforme
{
	////////////////////
	// Propiedades
	private:
	std::vector<T> items;

	////////////////////
	// Interface pública
	public:

	Selector_aleatorio_uniforme() {}

	void reset()
	{
		items.clear();
	}

	/**
	* @param const T& o
	* @param unsigned int p : como el porcentaje de posibilidades de que aparezca.
	*/

	void insertar(const T& o)
	{
		items.push_back(o);
	}

	/**
	* @throw SelectorAleatorioException
	* Obtiene un elemento aleatorio.
	*/

	const T& obtener() const
	{
		size_t tam=items.size();
		if(!tam) throw SelectorAleatorioException("El vector está vacío", 0);
		else if(tam==1) return items[0];
		
		Generador_int G(0, tam-1);
		size_t val=G();
		return items[val];
	}

	size_t size() const
	{
		return items.size();
	}
};

}

#endif
