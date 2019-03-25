#ifndef TOOLS_ALGORITHM_H
#define TOOLS_ALGORITHM_H

#include <functional> 

namespace tools {

//!Reduces the range begin-end to a value of rtype (with an initial value of
//!_initial) calling the callback _lambda, which will accept the element the
//!iterators point at.
template <typename iterator, typename lambda, typename rtype>
rtype reduce(iterator _begin, iterator _end, lambda _callback, rtype initial) {

	while(_begin != _end) {
		//TODO: Actually, the "+" operation is the one thing I don't like...
		//having + here is limiting our options.
		initial+=_callback(*_begin);
		++_begin;
	}

	return initial;
};

}

#endif
