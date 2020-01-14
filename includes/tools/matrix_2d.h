#pragma once
#include "compatibility_patches.h"
#include <map>
#include <stdexcept>
#include <string>

namespace tools{

//!Base exception for all exceptions thrown by the matrix_2d.
struct matrix_2d_exception:
	public std::runtime_error 
{
	int 			x, 	//!< X coordinate where the problemm originated.
				y;	//!< Y coordinate where the problemm originated.
	//!Class constructor.
	matrix_2d_exception(int px, int py, const std::string& msg):
		std::runtime_error("matrix_2d error: "+msg), x(px), y(py) {
	}
};

//!Thrown when trying to access, insert, erase or check an element out of the 
//!matrix bounds.
struct matrix_2d_exception_bounds:
	public matrix_2d_exception 
{
	//!Class constructor.
	matrix_2d_exception_bounds(int px, int py)
		:matrix_2d_exception(
			px, py, 
			std::string("out of bounds ")+compat::to_string(px)+","+compat::to_string(py)) {
	}
};

//!Thrown when trying to erase or access a non existing element.
struct matrix_2d_exception_missing:
	public matrix_2d_exception 
{
	//!Class constructor.
	matrix_2d_exception_missing(int px, int py)
		:matrix_2d_exception(
			px, py,
			std::string("no value ")+compat::to_string(px)+","+compat::to_string(py)) {
	}
};

//!Thrown when trying to insert an item in a position already occupied.
struct matrix_2d_exception_conflict:
	public matrix_2d_exception 
{
	//! Class constructor.
	matrix_2d_exception_conflict(int px, int py)
		:matrix_2d_exception(
			px, py,
			std::string("insertion conflict ")+compat::to_string(px)+","+compat::to_string(py)) {
	}
};

//!Struct to encapsulate a pair of coordinates and an element of type T. This
//!type is only a userland complement to the matrix and it is never returned
//!from its methods.
template<typename T>
struct matrix_2d_item {
	unsigned int 		x, 		//!< X coordinate.
				y;		//!< Y coordinate.
	T 			elem;		//!< Stored element.

	//!Class constructor.
				matrix_2d_item(unsigned int px, unsigned int py, T e): x(px), y(py), elem(e) {
	}

	//!Class constructor.
				matrix_2d_item(unsigned int px, unsigned int py, T&& e): x(px), y(py), elem(std::move(e)) {
	}
};

//!Implements a 2d matrix with fixed dimensions in terms of a std::map, so 
//!storage is used only for the inserted items. Its implementation only
//!allows for positive indexes.

template<typename T>
class matrix_2d {

	public:

	//!Constructs a matrix with the given dimensions.
					matrix_2d(unsigned int pw, unsigned int ph)
		:w(pw), h(ph) {

	}

	//!Copy-constructs a matrix,
					matrix_2d(const matrix_2d& o)
		:data(o.data), w(o.w), h(o.h) {

	}

	//!Inserts val into the x, y position. Might throw if already occupied.
	void 				insert(unsigned int x, unsigned int y, T& val) {
		unsigned int index=coords_to_index(x, y);
		if(data.count(index)) throw matrix_2d_exception_conflict(x, y);
		data.insert(std::make_pair(index, val));
	}

	//!Inserts val into the x, y position. Might throw if already occupied.
	void 				insert(unsigned int x, unsigned int y, T&& val) {
		unsigned int index=coords_to_index(x, y);
		if(data.count(index)) throw matrix_2d_exception_conflict(x, y);
		data.insert(std::make_pair(index, val));
	}

	//!Removes the element at x, y. Might throw if there's nothing there.
	void 				erase(unsigned int x, unsigned int y) {
		unsigned int index=coords_to_index(x, y);
		if(!data.count(index)) throw matrix_2d_exception_missing(x, y);
		data.erase(index);
	}

	//TODO: Provide a "replace" operator.
	//TODO: Provide swap capabilities.

	//!Returns the element at x, y. Might throw. 
	const T& 			operator()(unsigned int x, unsigned int y) const {
		unsigned int index=coords_to_index(x, y);
		if(!data.count(index)) throw matrix_2d_exception_missing(x, y);
		return data.at(index);
	}

	//!Returns the element at the given coordinates Throws if no item is present.
	T& 				operator()(unsigned int x, unsigned int y) {
		unsigned int index=coords_to_index(x, y);
		if(!data.count(index)) throw matrix_2d_exception_missing(x, y);
		return data.at(index);
	}

	//!Inserts val into x, y. Will throw if the position is occupied.
	T& 				operator()(unsigned int x, unsigned int y, T& val) {
		unsigned int index=coords_to_index(x, y);
		if(data.count(index)) throw matrix_2d_exception_conflict(x, y);
		data.insert(std::make_pair(index, val));
		return data.at(index);
	}

	//!Inserts val into x, y. Will throw if the position is already occupied.
	T& 				operator()(unsigned int x, unsigned int y, T&& val) {
		unsigned int index=coords_to_index(x, y);
		if(data.count(index)) throw matrix_2d_exception_conflict(x, y);
		data.insert(std::make_pair(index, val));
		return data.at(index);
	}

	//!Returns true if there is something in the given coordinates. Does
	//!not do bound checking, so it might throw.
	bool 				count(unsigned int x, unsigned int y) const {
		return data.count(coords_to_index(x, y));
	}

	//!Checks existence of a T in the given coordinates and returns true if
	//!something can be found. False is returned when there's nothing or when
	//!the coordinates are out of bounds.
	bool 				check(unsigned int x, unsigned int y) const {
		if(x >= w || y >= h) {
			return false; 
		}
		else {
			return this->count(x, y);
		}
	}

	//!Returns the count of T items.
	size_t 				size() 	const {return data.size();}

	//!Clears all T items from the matrix.
	void				clear() {data.clear();}

	//!Returns the width of the matrix.
	unsigned int			get_w() const {return w;}

	//!Returns the height of the matrix.
	unsigned int			get_h() const {return h;}
	
	//!Resizes the matrix to pw x ph. Items outside the range of the new
	//!size (if smaller) are removed.
	void				resize(unsigned int pw, unsigned int ph) {

		if(pw==w && ph==h) return;

		//We need to remember the old values to perform index_to_coords.
		unsigned int ow=w, oh=h;

		//Reassign...
		w=pw;
		h=ph;

		//Save some cycles and data.
		if(!data.size()) return;

		//This is the real final container.
		std::map<unsigned int, T> 	new_data;

		//Iterate and reinsert those whitin range.
		for(const auto& p : data) {
			auto c=index_to_coords(p.first, ow, oh);
			if(c.x < w && c.y < h) {
				new_data.insert(std::make_pair(coords_to_index(c.x, c.y), p.second));
			}
		}

		//Swap.
		std::swap(new_data, data);
	}

	//!Returns a new matrix from the current one, resized to pw x ph. 
	//!Particularities of "resize" apply.
	matrix_2d<T> 			copy_and_resize(unsigned int pw, unsigned int ph) const
	{
		matrix_2d<T> result(pw, ph);

		for(const auto& p : data) {
			auto c=index_to_coords(p.first);
			auto copia=p.second;
			if(c.x < result.w && c.y < result.h) result(c.x, c.y, copia);
		}
		return result;
	}

	//!Applies the function/functor f to every item in the matrix.
	template <typename TipoFunc> 
	void 				apply(TipoFunc& f) const {
		for(auto& p : data) {
			f(p.second);
		}
	}

	private:

	//!Defines a pair of coordinates.
	struct coords {
		unsigned int 		x,	//!< Coordinate x.
					y;	//!< Coordinate y.
		coords(unsigned int px, unsigned int py): x(px), y(py) {} //!< Class constructor.
	};

	std::map<unsigned int, T> 	data;	//!< Internal storage.

	unsigned int 			w, 	//!< Matrix width.
					h;	//!< Matrix height.

	private:

	//!Converts the coordinates to a map index.
	unsigned int 			coords_to_index(unsigned int x, unsigned int y) const {
		if(x >= w || y >= h) throw matrix_2d_exception_bounds(x, y);
		return (y * w) + x;
	}

	//Converts a map index to coordinates.
	coords 				index_to_coords(unsigned int index) const {
		int y=index / w;
		int x=index % w;
		return coords(x, y);
	}

	//!Converts a map index to coordinates, considering pw and ph as width and height of the matrix.
	coords 				index_to_coords(unsigned int index, unsigned int pw, unsigned int ph) const {
		int y=index / ph;
		int x=index % pw;
		return coords(x, y);
	}
};

}
