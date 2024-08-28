#pragma once
#include <cstdlib>

namespace tools
{

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

}

