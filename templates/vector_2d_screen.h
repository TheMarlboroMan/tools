#ifndef TOOLS_VECTOR_SCREEN_H
#define TOOLS_VECTOR_SCREEN_H

#include <tools/vector_2d/vector_2d.h>

namespace tools
{

//!This vector is structured according to screen coordinates.

//!Negative Y o is up, positive Y is down. It is pretty much the same as the
//!previous one save for the angle functions. The angle_for_vector_X functions
//!are not virtual, and this is a derived template from vector_2d, so if they
//!are called from a pointer to the base they will execute the base code.

template<typename T>
struct vector_2d_screen:
	public ldt::vector_2d<T>
{
	//Thanks c++11.
	using ldt::vector_2d<T>::vector_2d;

	//!Gets angle in radians. It is not virtual, so do not call on pointers.
	T 					angle_rad() const
	{
		return angle_for_vector_rad({this->x, -this->y});
	}

	//!Gets angle in degrees. It is not virtual, so do not call on pointers.
	T 					angle_deg() const
	{
		return angle_for_vector_deg({this->x, -this->y});
	}
};

//!Converts a screen vector to a cartesian one.
template<typename T>
vector_2d<T> screen_to_vector(const vector_2d_screen<T>& v)
{
	return vector_2d<T>(v.x, -v.y);
}

//!Converts a cartesian vector to a screen one. Actually, the same code as its inverse.
template<typename T>
vector_2d_screen<T> vector_to_screen(const vector_2d<T>& v)
{
	return vector_2d_screen<T>(v.x, -v.y);
}
}

#endif
