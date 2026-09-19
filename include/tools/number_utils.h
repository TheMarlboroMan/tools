#pragma once
#include <algorithm>

namespace tools {

//!Returns the number of digits of the given integer with the most puerile algorithm possible.
size_t					count_digits(int);

/**
 * calculates the percent that "_part" corresponds of "_total"
 */
template<typename T>
T percent(
	T _part, 
	T _total,
	int _percent=100
) {
	T zero{};

	return _total==zero
		? zero
		: (_part * (T)_percent) / _total;
}

/**
 * linear interpolation of _start and _end according to time _t, where _t
 * is between 0. (start) and 1.0.(end). _t is not clamped at all and can go
 * outside the range to surprising effects.
 */
template<typename T>
T lerp(T _start, T _end, T _time) {

	return _start + ((_end - _start) * _time);
}

/**
* linear interpolation of _start and _end according to time _t, where _t is
* a clamped value between zero (start) and one (end).
*/
template<typename T>
T lerp_clamp(T _start, T _end, T _time) {

	auto clamp_time=std::clamp(_time, static_cast<T>(0.0), static_cast<T>(1.0));
	return _start + ((_end - _start) * clamp_time);
}

} //end of namespace
