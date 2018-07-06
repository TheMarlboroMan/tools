#ifndef TOOLS_LINEAR_VALUE_H
#define TOOLS_LINEAR_VALUE_H

#include <algorithm>

#include "ranged_value.h"

//!A generic solution for ranged values that evolve in time in a linear way: useful
//!for linear transparencies, animations and such.

/*!
To use the class:

- Create a instance from the static methods.
- Call "tic" whenever there's elapset time. Tic takes a delta time, which till
	take the value closer to its limit. The method is safe and can be called
	even when the linear value has reached its limit.
- The operator () will return the current value (can be used just by naming
	the instance).
- is_done() will return true if the value has reached its limit.
- reset() can be called to reset the instance. It takes the same parameters as
	the constructor.
- Time is expressed in milliseconds.
*/

namespace tools
{

template <typename T>
class linear_timed_function {
	public:

	//!Default constructs the instance. This is useless without a call to reset.
			linear_timed_function():
				val(0, 0, 0), target(0), step(0)
	{

	}

	//!Constructs the instance given the current value, the target and the step (delta per second).
	static linear_timed_function	from_step(T v, T w, T step) {
		return linear_timed_function(v, w, step);
	}

	//!Constructs the instance given the current value, the targe and the 
	//!time in seconds that it should take. Will throw if time is zero.
	static linear_timed_function	from_time(T v, T dest, T time)
	{
		if(!time) throw std::runtime_error("linear_timed_function cannot be constructed without time");
		return linear_timed_function(v, dest, (dest-v) / time);
	}

	//!Returns the current value.
	T 				operator()() const {return val;}
	//!Returns the current value.
					operator T() const {return val;}

	//!Resets the instance with a as the current value, d as the target and s as step.
	void		reset(T a, T d, T s) {
		val=tools::ranged_value<T>(std::min(a, d), std::max(a, d), a);
		target=d;
		step=s;
	}

	//!Resets the instance with the current value, target and new time it will take (in seconds).
	void		reset_from_time(T a, T d, T t) {
		val=tools::ranged_value<T>(std::min(a, d), std::max(a, d), a);
		target=d;
		step=(d-a) / t;
	}

	//!Makes time pass. Calculates the new current value after delta milliseconds have passed.
	void 		tic(float delta) {
		T sal=delta * step;
		val+=sal;
	}

	//!Returns true when the current value has reached its target.
	bool		is_done() const {return val==target;}

	
	private:

	//!Private constructor: this class is either build empty or from named constructors.
			linear_timed_function(T a, T d, T s):
				val(std::min(a, d), std::max(a, d), a),
				target(d), step(s) {

	}


	tools::ranged_value<T>			val;		//<! Current value.
	T					target,		//<! Target.
						step;		//<! Units that value will be closer to target after one second.
};
}

#endif
