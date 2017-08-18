#ifndef TOOLS_MATRIX_2D_UNBOUND_H
#define TOOLS_MATRIX_2D_UNBOUND_H

#include <map>
#include <stdexcept>
#include <string>

#include "compatibility_patches.h"

namespace tools
{

struct matrix_2d_unbound_exception:
	public std::runtime_error
{
	int 			x, 
				y;
	matrix_2d_unbound_exception(int px, int py, const std::string& msg):
		std::runtime_error("matrix_2d_unbound error: "+msg), x(px), y(py) 
	{
	}
};

struct matrix_2d_unbound_exception_missing:
	public matrix_2d_unbound_exception 
{
	matrix_2d_unbound_exception_missing(int px, int py)
		:matrix_2d_unbound_exception(
			px, py,
			std::string("no value ")+compat::to_string(px)+","+compat::to_string(py)
		) {}
};


struct matrix_2d_unbound_exception_conflict:
	public matrix_2d_unbound_exception 
{
	matrix_2d_unbound_exception_conflict(int px, int py)
		:matrix_2d_unbound_exception(
			px, py,
			std::string("insertion conflict ")+compat::to_string(px)+","+compat::to_string(py)
		) {}
};

template<typename T>
struct matrix_2d_unbound_item
{
	int x, y;
	T elem;

	matrix_2d_unbound_item(int px, int py, T e): x(px), y(py), elem(e) 
	{}

	matrix_2d_unbound_item(int px, int py, T&& e): x(px), y(py), elem(std::move(e))
	{}
};

template<typename T>
class matrix_2d_unbound
{
	public:

	typedef int 			tscalar;
	typedef std::pair<coords, T>	tpair;

	struct coords{
		tscalar x, y;
		bool operator<(const coords& o) const
		{
			if(x < o.x) return true;
			else if(x > o.x) return false;
			else return y < o.y;
		}
	};

					matrix_2d_unbound()
	{

	}

					matrix_2d_unbound(const matrix_2d_unbound& o)
		:data(o.data)
	{

	}

	void 				insert(tscalar x, tscalar y, T& val)
	{
		auto index=make_index(x, y);
		if(data.count(index)) throw matrix_2d_unbound_exception_conflict(x, y);
		data.insert(std::make_pair(index, val));
	}

	void 				insert(tscalar x, tscalar y, T&& val)
	{
		auto index=make_index(x, y);
		if(data.count(index)) throw matrix_2d_unbound_exception_conflict(x, y);
		data.insert(std::make_pair(index, val));
	}

	void 				erase(tscalar x, tscalar y)
	{
		auto index=make_index(x, y);
		if(!data.count(index)) throw matrix_2d_unbound_exception_missing(x, y);
		data.erase(index);
	}

	//Se usa "at" para evitar la necesidad de constructores por defecto.
	const T& 			operator()(tscalar x, tscalar y) const
	{
		auto index=make_index(x, y);
		if(!data.count(index)) throw matrix_2d_unbound_exception_missing(x, y);
		return data.at(index);
	}

	T& 				operator()(tscalar x, tscalar y)
	{
		auto index=make_index(x, y);
		if(!data.count(index)) throw matrix_2d_unbound_exception_missing(x, y);
		return data.at(index);
	}

	//Permite inserciones...
	T& 				operator()(tscalar x, tscalar y, T& val)
	{
		auto index=make_index(x, y);
		if(data.count(index)) throw matrix_2d_unbound_exception_conflict(x, y);
		data.insert(std::make_pair(index, val));
		return data.at(index);
	}

	T& 				operator()(tscalar x, tscalar y, T&& val)
	{
		auto index=make_index(x, y);
		if(data.count(index)) throw matrix_2d_unbound_exception_conflict(x, y);
		data.insert(std::make_pair(index, val));
		return data.at(index);
	}


	//The very same...
	bool 				count(tscalar x, tscalar y) const {return data.count(make_index(x, y));}
	bool 				check(tscalar x, tscalar y) const {return data.count(make_index(x, y));}
	size_t 				size() 	const {return data.size();}
	void				clear() {data.clear();}
	
	//Absolutely anything that has an operator () or even a function can be used with this :).
	template <typename TipoFunc> 
	void 				apply(TipoFunc& f) const
	{
		for(auto& p : data) f(p.second);
	}
	
	//The same as before, where f is executed in a std::pair<coord, t> or tpair.
	template <typename TipoFunc> 
	void 				apply_pair(TipoFunc& f) const
	{
		for(auto& p : data) f(p);
	}

	private:

	std::map<coords, T> 	data;

	coords				make_index(tscalar x, tscalar y) const
	{
		return {x, y};
	}
};

}
#endif
