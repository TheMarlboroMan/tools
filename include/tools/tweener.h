#pragma once
#include "tweener_interpolation_interface.h"

namespace tools {
namespace tween {

//!Basic tweener.
template<typename U>
class tweener {

	public:

	//!Creates a tweener that will take _from to _to along _time seconds using
	//!the given interpolation function
	            tweener(U _from, U _to, double _time, interpolators::interpolating_function_interface& _interpolator)
	            :from{_from}, to{_to}, current_time{0}, duration{_time}, interpolator{&_interpolator}
	{ }

	//!Tics the tweener, returns the new interpolated value for this moment in time.
	U           tic(double _time) {

		current_time+=_time;

		//Clamp current time to be between 0. and duration.
		if(current_time > duration) {

			current_time=duration;
		}

		double double_delta=interpolator->update(current_time / duration);
		U delta=static_cast<U>(double_delta);
		return from + (delta * (to - from));
	}

	//!Resets the tweener to use new origins, target and duration. Resets the
	//!current timer to zero.
	tweener&    reset(U _from, U _to, double _time) {

		from=_from;
		to=_to;
		duration=_time;
		current_time=0.;
		return *this;
	}

	//!Changes the interpolator.
	tweener&    set_interpolator(interpolators::interpolating_function_interface& _interpolator) {

		interpolator=&_interpolator;
		return *this;
	}

	//!Returns true when the timer has reached its target.
	bool        is_finished() const {

		return current_time == duration;
	}

	//!Returns the current timer,
	double      get_current_time() const {

		return current_time;
	}

	//!Returns the target for the timer
	double      get_duration() const {

		return duration;
	}

	//!Returns the origin value to transform.
	U           get_from() const {

		return from;
	}

	//!Returns the target transformation for the value.
	U           get_to() const {

		return to;
	}

	private:

	U                       from,
                            to;
	double                  current_time,
	                        duration;
	interpolators::interpolating_function_interface* interpolator;
};

}} //end of namespaces
