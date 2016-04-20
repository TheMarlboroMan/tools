#ifndef VALOR_LINEAL_H
#define VALOR_LINEAL_H

#include <algorithm>

#include <class/valor_limitado.h>

/*
Una solución genérica a valores que van desde un inicio hasta un fin en un 
tiempo.

- Se crea el objeto a partir de los métodos estáticos

- Se llama al método tic(delta) pasándo el intervalo de tiempo. Con cada 
turno, el valor se acercará más al valor de destino. Es seguro llamar al 
método aunque se haya llegado al destino.

- Ejecutar el operador () (por ejemplo, nombrando el objeto) indicará el valor 
actual.

- Llamar a es_finalizado() devolverá verdadero si se ha alcanzado el destino.

- Se puede llamar a reset con los mismos parámetros que el constructor para
asignar nuevos valores al objeto.
*/

namespace Herramientas_proyecto
{
template <typename T>
class Valor_lineal
{
	public:

			Valor_lineal():
				val(0, 0, 0), destino(0), salto(0)
	{

	}

	static Valor_lineal	desde_salto(T v, T d, T s)
	{
		return Valor_lineal(v, d, s);
	}

	static Valor_lineal	desde_tiempo(T v, T d, T t)
	{
		return Valor_lineal(v, d, (d-v) / t);
	}

	operator T() const{return val;}

	void		reset(T a, T d, T s)
	{
		val=Herramientas_proyecto::Valor_limitado<T>(std::min(a, d), std::max(a, d), a);
		destino=d;
		salto=s;
	}

	void 		tic(float delta)
	{
		T sal=delta * salto;
		if(val > destino) 
		{
			val-=sal;
		}
		else if(val < destino) 
		{
			val+=sal;
		}
	}

	bool		es_finalizado() const {return val==destino;}

	private:
			Valor_lineal(T a, T d, T s):
				val(std::min(a, d), std::max(a, d), a),
				destino(d), salto(s)
	{

	}


	Herramientas_proyecto::Valor_limitado<T>	val;
	T						destino,
							salto;
};
}

#endif
