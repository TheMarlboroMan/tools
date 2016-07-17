#ifndef TOOLS_BASE_LIST_H
#define TOOLS_BASE_LIST_H

#include <iostream>
#include <vector>
#include "../class/pager.h"

namespace tools
{

template<typename T>
class base_list
{
	public:

	void					clear()
	{
		data.clear();
		pager_instance.set_item_count(0);
		pager_instance.set_index(0);
	}

	void 					insert(const T& v)
	{
		data.push_back(v);
		pager_instance.set_item_count(pager_instance.get_item_count()+1);
	}

	size_t					size() const 
	{
		return data.size();
	}

	const T&				operator[](size_t v) const 
	{
		return data[v];
	}

	T&					operator[](size_t v)
	{
		return data[v];
	}

	const T&				get() const 
	{
		return data[pager_instance.get_current_index()];
	}

	T&					get()
	{
		return data[pager_instance.get_current_index()];
	}

	//Passthroughs paginación...
	void					reset_index() 	{pager_instance.reset_index();}
	void					set_index(size_t v)	{pager_instance.set_index(v);}
	bool					set_page(int v) 	{return pager_instance.set_page(v);}
	bool					set_item(int v)	{return pager_instance.set_item(v);}
	size_t					get_current_page() const {return pager_instance.get_current_page();}
	size_t					get_current_index() const {return pager_instance.get_current_index();}
	size_t					get_pages_count() const {return pager_instance.get_pages_count();}
	size_t					get_items_per_page() const {return pager_instance.get_items_per_page();}

	protected:

	std::vector<T>				data;
	pager					pager_instance;

};
}

#endif
