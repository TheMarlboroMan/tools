#pragma once

#include "pager.h"

#include <iostream>
#include <vector>

namespace tools
{

//!Base class for vertical or grid lists.

//!As much as it can be instantiated, it's useless by itself. It is built on
//!top of a pager, so there is a currently selected element, a current page,
//!etc.
template<typename T>
class base_list
{
	public:

	//!Removes all elements from the internal list.
	void					clear() {
		data.clear();
		pager_instance.set_item_count(0);
		pager_instance.set_index(0);
	}

	//!Inserts an element at the end of the list.
	void 					insert(const T& v) {
		data.push_back(v);
		pager_instance.set_item_count(pager_instance.get_item_count()+1);
	}

	//!Selects the next item...
	void					next() {

		pager_instance->cycle_item(pager::dir::next);
	}

	//!Selects the next item...
	void					previous() {

		pager_instance->cycle_item(pager::dir::previous);
	}
	
	//!Returns the size of the list.
	size_t					size() const {
		return data.size();
	}

	//!Returns the element at the given position. Does not do boundary checking.
	const T&				operator[](size_t v) const {
		return data[v];
	}

	//!Returns a reference to the  element at the given position. Does not do boundary checking.
	T&					operator[](size_t v) {
		return data[v];
	}

	//!Returns the element being pointed at by the pager component.
	const T&				get() const {
		return data[pager_instance.get_current_index()];
	}

	//!Returns the element being pointed at by the pager component.
	T&					get() {
		return data[pager_instance.get_current_index()];
	}

	//!Passthrough for pager.reset_index().
	void					reset_index() 	{pager_instance.reset_index();}

	//!Passthrough for pager.set_index().
	void					set_index(size_t v)	{pager_instance.set_index(v);}

	//!Passthrough for pager.get_current_page().
	size_t					get_current_page() const {return pager_instance.get_current_page();}

	//!Passthrough for pager.get_current_index().
	size_t					get_current_index() const {return pager_instance.get_current_index();}

	//!Passthrough for pager.get_pages_count().
	size_t					get_page_count() const {return pager_instance.get_pages_count();}

	//!Passthrough for pager.get_items_per_page().
	size_t					get_items_per_page() const {return pager_instance.get_items_per_page();}

	protected:

	std::vector<T>				data;			//!< Internal list of T.
	pager					pager_instance;		//!< Pager.
};
}

