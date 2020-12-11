#pragma once

#include "pager.h"

#include <algorithm>
#include <limits>
#include <functional>
#include <iostream>
#include <vector>
#include <stdexcept>

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
	void                    clear() {
		data.clear();
		pager_instance.set_item_count(0);
		pager_instance.set_index(0);
	}

	//!Inserts an element at the end of the list.
	void                    insert(const T& v) {
		data.push_back(v);
		pager_instance.set_item_count(pager_instance.get_item_count()+1);
	}

	//!Selects the next item...
	void                    next() {

		pager_instance.cycle_item(pager::dir::next);
	}

	//!Selects the next item...
	void                    previous() {

		pager_instance.cycle_item(pager::dir::previous);
	}

	//!Returns the size of the list.
	std::size_t             size() const {
		return data.size();
	}

	//!Returns the element at the given position. Does not do boundary checking.
	const T&                operator[](std::size_t v) const {
		return data[v];
	}

	//!Returns a reference to the  element at the given position. Does not do boundary checking.
	T&                      operator[](std::size_t v) {
		return data[v];
	}

	//!Returns the element being pointed at by the pager component.
	const T&                get() const {
		return data[pager_instance.get_current_index()];
	}

	//!Returns the element being pointed at by the pager component.
	T&                      get() {
		return data[pager_instance.get_current_index()];
	}

	//!Passthrough for pager.reset_index().
	void                    reset_index() 	{pager_instance.reset_index();}

	//!Passthrough for pager.set_index().
	void                    set_index(std::size_t v) {pager_instance.set_index(v);}

	//!Passthrough for pager.get_current_page().
	std::size_t             get_current_page() const {return pager_instance.get_current_page();}

	//!Passthrough for pager.get_current_index().
	std::size_t             get_current_index() const {return pager_instance.get_current_index();}

	//!Passthrough for pager.get_pages_count().
	std::size_t             get_page_count() const {return pager_instance.get_pages_count();}

	//!Passthrough for pager.get_items_per_page().
	std::size_t             get_items_per_page() const {return pager_instance.get_items_per_page();}

	//!Returns true if the item is present.
	bool                    has(const T& _item) const {

		return std::find(std::begin(data), std::end(data), _item)!=std::end(data);
	}

	//!Returns the index of the given item. returns "none" if cannot be found.
	std::size_t             get_index_of(const T& _item) const {

		auto it=std::find(std::begin(data), std::end(data), _item);
		if(it == std::end(data)) {

			return none;
		}

		return std::distance(std::begin(data), it);
	}

	//!Tries to locate the first item that satisfies the predicate _fn, returns
	//!"none" if nothing satisfies the predicate or its index if it does.
	std::size_t             find(std::function<bool(const T&)> _fn) {

		auto it=std::find_if(std::begin(data), std::end(data), _fn);
		if(it == std::end(data)) {

			return none;
		}

		return std::distance(std::begin(data), it);
	}

	const std::size_t       none{std::numeric_limits<std::size_t>::max()};

	protected:

	std::vector<T>          data;			//!< Internal list of T.
	pager                   pager_instance;		//!< Pager.
};
}

