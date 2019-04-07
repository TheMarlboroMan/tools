#ifndef TOOLS_ALGORITHM_H
#define TOOLS_ALGORITHM_H

#include <functional>

namespace tools {

//!Reduces the range begin-end to a value of rtype (with an initial value of
//!_initial) calling the callback _lambda, which will accept a reference to
//!initial and the current item to compupte its reduction.
template <typename iterator, typename lambda, typename rtype>
rtype reduce(iterator _begin, iterator _end, lambda _callback, rtype&& initial) {

	rtype result{initial}; //The callback will not accept a reference to initial if it is an rvalue.

	while(_begin != _end) {
		_callback(result, *_begin);
		++_begin;
	}

	return result;
}

}

#endif
