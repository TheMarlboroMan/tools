#ifndef TOOLS_LINEAR_VALUE_H
#define TOOLS_LINEAR_VALUE_H

#include <algorithm>

#include "ranged_value.h"

/*
Una solución genérica a valores que van from un inicio hasta un fin en un 
time.

- Se crea el objeto a partir de los métodos estáticos

- Se llama al método tic(delta) pasándo el intervalo de time. Con cada 
turno, el valor se acercará más al valor de target. Es seguro llamar al 
método aunque se haya llegado al target.

- Ejecutar el operador () (por ejemplo, nombrando el objeto) indicará el valor 
actual.

- Llamar a is_done() devolverá verdadero si se ha alcanzado el target.

- Se puede llamar a reset con los mismos parámetros que el constructor para
asignar nuevos valores al objeto.
*/

namespace tools
{
template <typename T>
class linear_timed_function
{
	public:

			linear_timed_function():
				val(0, 0, 0), target(0), step(0)
	{

	}

	//Where v is one extreme (min or max) and dest is the other.
	static linear_timed_function	from_step(T v, T dest, T step)
	{
		return linear_timed_function(v, dest, step);
	}

	//Same as before.
	static linear_timed_function	from_time(T v, T dest, T time)
	{
		return linear_timed_function(v, dest, (dest-v) / time);
	}

	T 				operator()() const {return val;}

	void		reset(T a, T d, T s)
	{
		val=tools::ranged_value<T>(std::min(a, d), std::max(a, d), a);
		target=d;
		step=s;
	}

	void 		tic(float delta)
	{
		T sal=delta * step;
		val+=sal;
	}

	bool		is_done() const {return val==target;}

	private:
			linear_timed_function(T a, T d, T s):
				val(std::min(a, d), std::max(a, d), a),
				target(d), step(s)
	{

	}


	tools::ranged_value<T>			val;
	T					target,
						step;
};
}

#endif
