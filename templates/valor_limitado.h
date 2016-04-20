#ifndef VALOR_LIMITADO_H
#define VALOR_LIMITADO_H

#include <iostream>

namespace Herramientas_proyecto
{

template <typename T>
class Valor_limitado
{
	public:

	enum tlim{superior=1, inferior=2, ambos=3};

			Valor_limitado(T vmin, T vmax, T vact, int l=ambos):
				valor_min(vmin),
				valor_max(vmax),
				valor_actual(vact),
				limites(l)
			{}

	T		min() const {return valor_min;}
	T		min(const T v) {valor_min=v;}

	T		max() const {return valor_max;}
	T		max(const T v) {valor_min=v;}

	T		actual() const {return valor_actual;}
	T		actual(const T v) 
	{
		return valor_actual=v; regular();
	}
	
	Valor_limitado& operator+=(const T v)
	{
		this->operator+(v);
		return *this;
	}

	Valor_limitado& operator-=(const T v)
	{
		this->operator-(v);
		return *this;
	}

	operator T() const
	{
		return valor_actual;
	}

	void		operator+(const T v) 
	{
		valor_actual+=v;
		regular();
	}

	void		operator-(const T v) 
	{
		valor_actual-=v;
		regular();
	}


	Valor_limitado&	operator=(const T v)
	{
		valor_actual=v;
		regular();
		return *this;
	}

	bool operator>(const Valor_limitado& otro) const
	{
	    	return valor_actual > otro.actual;
	}

	bool operator>(const T v) const
	{
	    	return valor_actual > v;
	}

	bool operator<(const Valor_limitado& otro) const
	{
	    	return valor_actual < otro.valor_actual;
	}

	bool operator<(const T v) const
	{
	    	return valor_actual < v;
	}

	bool operator>=(const Valor_limitado& otro) const
	{
	    	return valor_actual >= otro.valor_actual;
	}

	bool operator>=(const T v) const
	{
	    	return valor_actual >= v;
	}

	bool operator<=(const Valor_limitado& otro) const
	{
	    	return valor_actual <= otro.valor_actual;
	}

	bool operator<=(const T v) const
	{
	    	return valor_actual <= v;
	}
 
	private:

	//Se pueden usar valores máximos menores que el mínimo y viceversa para
	//no limitar por algún lado.
	void		regular()
	{
		if(valor_actual > valor_max && limites & superior) 
		{
			valor_actual=valor_max;
		}
		else if(valor_actual < valor_min && limites & inferior) 
		{
			valor_actual=valor_min;
		}
	}

	T		valor_min,
			valor_max,
			valor_actual;
	int		limites;
};
}

#endif
