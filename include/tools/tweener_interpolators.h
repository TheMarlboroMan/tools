#pragma once
#include "tweener_interpolation_interface.h"
#include <cmath>

namespace tools {
namespace tween {
namespace interpolators {

inline constexpr double PI = 3.14159265358979323846;

//!Implementation of an interpolation function that is strictly linear in time.
struct linear:
	public interpolating_function_interface {

	double  update(double _time) {

		return _time;
	}
};

/**
 * quadratic ones, accelerate/deccelerate from/to zero.
 */
struct ease_in_quad:
	public interpolating_function_interface {

	double  update(double _time) {

		return _time * _time;
	}
};

struct ease_out_quad:
	public interpolating_function_interface {

	double  update(double _time) {

		double v = _time - 1.0;
		return 1. - (v * v);
	}
};

struct ease_in_out_quad:
	public interpolating_function_interface {

	double  update(double _time) {

		if (_time < 0.5) {

			return 2.0 * _time * _time;
		}

		double v = _time - 1.0;
		return 1.0 - 2.0 * v * v;
	}
};

/**
 * cubic ones, accelerate/deccelerate from/to zero.
 */
struct ease_in_cubic:
	public interpolating_function_interface {

	double  update(double _time) {

		return _time * _time * _time;
	}
};

struct ease_out_cubic:
	public interpolating_function_interface {

	double  update(double _time) {

		double v = _time - 1.0;
		return v * v * v + 1.0;
	}
};

struct ease_in_out_cubic:
	public interpolating_function_interface {

	double  update(double _time) {

		if(_time < 0.5) {

			return 4.0 * _time * _time * _time;
		}

		double v = (2.0 * _time) - 2.0;
		return 0.5 * v * v * v + 1.0;
	}
};

/*
 * sine ones are "gentle".
 */
struct ease_in_sine:
	public interpolating_function_interface {

	double update(double _time) {

		return 1.0 - std::cos((_time * PI) / 2.0);
	}
};

struct ease_out_sine:
	public interpolating_function_interface {

	double update(double _time) {

		return std::sin((_time * PI) / 2.0);
    }
};

struct ease_in_out_sine: 
	public interpolating_function_interface {
	double update(double _time) {

        return -(std::cos(PI * _time) - 1.0) / 2.0;
    }
};

/**
 * elastic, which go over the limit like a rubber band.
 */
struct ease_in_elastic: 
	public interpolating_function_interface {

	double update(double _time) {

		if (_time == 0.0) return 0.0;
		if (_time == 1.0) return 1.0;
		
		double c4 = (2.0 * PI) / 3.0;
		return -std::pow(2.0, 10.0 * _time - 10.0) * std::sin((_time * 10.0 - 10.75) * c4);
	}
};

struct ease_out_elastic:
	public interpolating_function_interface {

	double update(double _time) {

		if (_time == 0.0) return 0.0;
		if (_time == 1.0) return 1.0;

		double c4 = (2.0 * PI) / 3.0;
		return std::pow(2.0, -10.0 * _time) * std::sin((_time * 10.0 - 0.75) * c4) + 1.0;
    }
};

/**
 * like a basketball. If we wanted an easy in it could just be 1-result.
 */
struct ease_out_bounce: 
	public interpolating_function_interface {

	double update(double _time) {

		const double n1 = 7.5625;
		const double d1 = 2.75;

		if (_time < 1.0 / d1) {
			return n1 * _time * _time;
		} else if (_time < 2.0 / d1) {
			_time -= 1.5 / d1;
			return n1 * _time * _time + 0.75;
		} else if (_time < 2.5 / d1) {
			_time -= 2.25 / d1;
			return n1 * _time * _time + 0.9375;
		}

		_time -= 2.625 / d1;
		return n1 * _time * _time + 0.984375;
    }
};

}}} //End of namespaces
