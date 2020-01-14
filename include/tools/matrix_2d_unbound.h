#pragma once
#include "compatibility_patches.h"
#include <map>
#include <stdexcept>
#include <string>

namespace tools {

//!Base exception for matrix_2d_unbound
struct matrix_2d_unbound_exception:
	public std::runtime_error
{
	int 			x,	//!< X coordinate for the exception.
				y;	//!< Y coordinate for the exception.
	//!Class constructor.
	matrix_2d_unbound_exception(int px, int py, const std::string& msg):
		std::runtime_error("matrix_2d_unbound error: "+msg), x(px), y(py) {
	}
};

//!Exception for matrix_2d_unbound indicating that there is no value in the requested coordinates.
struct matrix_2d_unbound_exception_missing:
	public matrix_2d_unbound_exception 
{
	//!Class constructor.
	matrix_2d_unbound_exception_missing(int px, int py)
		:matrix_2d_unbound_exception(
			px, py,
			std::string("no value ")+compat::to_string(px)+","+compat::to_string(py)
		) {}
};

//!Exception for matrix_2d_unbound indicating that there is already a value in the coordinates where an insertion is attempted.
struct matrix_2d_unbound_exception_conflict:
	public matrix_2d_unbound_exception 
{
	//!Class constructor.
	matrix_2d_unbound_exception_conflict(int px, int py)
		:matrix_2d_unbound_exception(
			px, py,
			std::string("insertion conflict ")+compat::to_string(px)+","+compat::to_string(py)
		) {}
};

//!2d matrix representing a cell set with no fixed size bounds.

//!Both positive and negative coordinates can be used with it.

template<typename T>
class matrix_2d_unbound {

	public:

	typedef int 			tscalar;	//!< Typedef to the type used for the coordinates.

	//!Structure reprenting a coordinate pair, ordered first on the x axis.
	struct coords{
		tscalar x, 	//!< X coordinate.
			y;	//!< Y coordinate.

		//!Comparison operator, ordered first on the x axis, lesser to greater.
		bool operator<(const coords& o) const
		{
			if(x < o.x) return true;
			else if(x > o.x) return false;
			else return y < o.y;
		}
	};

	typedef std::pair<coords, T>	tpair;	//!< Typedef to map a pair of coordinates to a T type.

	//!Default constructor.
					matrix_2d_unbound() {}

	//!Creates a matrix_2d_unbound from another.
					matrix_2d_unbound(const matrix_2d_unbound& o)
		:data(o.data)
	{

	}

	//!Inserts the value in the coordinates. Will throw if there is a value present.
	void 				insert(tscalar x, tscalar y, T& val)
	{
		auto index=make_index(x, y);
		if(data.count(index)) throw matrix_2d_unbound_exception_conflict(x, y);
		data.insert(std::make_pair(index, val));
	}

	//!Inserts the value in the coordinates. Will throw if there is a value present.
	void 				insert(tscalar x, tscalar y, T&& val)
	{
		auto index=make_index(x, y);
		if(data.count(index)) throw matrix_2d_unbound_exception_conflict(x, y);
		data.insert(std::make_pair(index, val));
	}

	//!Removes the value in the coordinates. Will throw if there is no value present.
	void 				erase(tscalar x, tscalar y)
	{
		auto index=make_index(x, y);
		if(!data.count(index)) throw matrix_2d_unbound_exception_missing(x, y);
		data.erase(index);
	}

	//!Gets the element at the given coordinates. Will throw if there is no value present.
	const T& 			operator()(tscalar x, tscalar y) const
	{
		auto index=make_index(x, y);
		if(!data.count(index)) throw matrix_2d_unbound_exception_missing(x, y);
		return data.at(index);
	}

	//!Gets the element at the given coordinates. Will throw if there is no value present.
	T& 				operator()(tscalar x, tscalar y)
	{
		auto index=make_index(x, y);
		if(!data.count(index)) throw matrix_2d_unbound_exception_missing(x, y);
		return data.at(index);
	}

	//!Inserts the value at the given coordinates. Will throw is there is a value present.
	T& 				operator()(tscalar x, tscalar y, T& val)
	{
		auto index=make_index(x, y);
		if(data.count(index)) throw matrix_2d_unbound_exception_conflict(x, y);
		data.insert(std::make_pair(index, val));
		return data.at(index);
	}

	//!Inserts the value at the given coordinates. Will throw is there is a value present.
	T& 				operator()(tscalar x, tscalar y, T&& val)
	{
		auto index=make_index(x, y);
		if(data.count(index)) throw matrix_2d_unbound_exception_conflict(x, y);
		data.insert(std::make_pair(index, val));
		return data.at(index);
	}


	//!Checks if there is a value in the given coordinates.
	bool 				check(tscalar x, tscalar y) const {return data.count(make_index(x, y));}

	//!Returns the total amount of values in the matrix.
	size_t 				size() 	const {return data.size();}

	//!Clears the matrix.
	void				clear() {data.clear();}
	
	//!Executes the function or functor for each value in the matrix.
	template <typename Tf> 
	void 				apply(Tf& f) const {
		for(const auto& p : data) f(p.second);
	}
	
	//!Executes the function or functor for each pair (std::pair<coords, T>) in the matrix, giving access to the coordinates object.
	template <typename Tf> 
	void 				apply_pair(Tf& f) const {
		for(const auto& p : data) f(p);
	}

	private:

	std::map<coords, T> 	data;	//!< Internal representation of the stored data.

	//!Creates a coords object from two scalars.
	coords				make_index(tscalar x, tscalar y) const {return {x, y};}
};

}
