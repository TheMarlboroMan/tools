#pragma once

#include "base_list.h"

namespace tools {

//!Class to list items vertically, one on top of the other.
template<typename T>
class vertical_list:
	public base_list<T> {
	public:

	//!Represents an item in the list. The internal implementation does not
	//!maintain a list of items, but builds them when needed. The base_list,
	//!however, maintains a list of T.
	struct Item {
		size_t              y, 	//!< Y position.
							index;	//!< Index in the list.
		const T&            item;	//!< Reference to the underlying object.
	};

	//!Builds the list with the available height and height of each item.
	                        vertical_list(size_t available_h, size_t item_h)
		:available_h(available_h), item_h(item_h), margin_h(0) {
		this->pager_instance.set_items_per_page(floor(available_h / item_h));
	}

	//!Returns the currently selected item.
	const Item              current_line() const {
		size_t i_actual=this->pager_instance.get_current_index();
		size_t index=i_actual % this->pager_instance.get_items_per_page();

		return Item{
			(item_h * index) + (margin_h * index),
			i_actual,
			this->get()
		};
	}

	//!Returns the line at the given index.
	//TODO: Does not check for index validity!.
	const Item              line(size_t i) const{
		return Item{(item_h * i) + (margin_h * i),
				this->pager_instance.get_current_index(),
				this->data[i]};
	}

	//!Returns a vector of items representing a page (as much items as
	//!can fit into the available height).
	//!Depends on the "pager" attached to the list, so it returns the
	//!pager's currently selected page.
	std::vector<Item>       get_page() const {
		std::vector<Item> res;
		size_t rpp=this->pager_instance.get_items_per_page();
		size_t pa=this->pager_instance.get_current_page();

		auto	ini=std::begin(this->data)+(pa * rpp),
			fin=ini+rpp;

		size_t y=0, index=pa*rpp;

		while(ini < fin && ini < std::end(this->data)) {
			res.push_back(Item{y, index++, *ini});
			++ini;
			y+=this->item_h+margin_h;
		}

		return res;
	}

	//!Locates the item in the ry vertical coordinate and calls "f" with
	//!the item on that position. Will return false if no item can be
	//!found, true if something is there.
	template<typename E>
	bool                    topological_select(size_t ry, E f) {

		const auto& pag=get_page();
		for(const auto& itemp : pag) {
			if(ry >= itemp.y && ry <= itemp.y + this->item_h)
			{
				f(itemp);
				return true;
			}
		}

		return false;
	}

	//!Returns the height of each item.
	size_t                  get_item_h() const {return item_h;}

	//!Returns the vertical margin between items.
	size_t                  get_margin_h() const {return margin_h;}

	//!Sets the vertical margin between items.
	void                    set_margin_h(size_t v) {

		margin_h=v;
		rebuild();
	}

	//!Sets the available height for the list. Triggers a pager rebuild.
	void                    set_available_h(size_t v) {

		available_h=v;
		rebuild();
	}

	//!Sets the height of each item. Triggers a pager rebuild.
	void                    set_item_h(size_t v) {

		item_h=v;
		rebuild();
	}

	private:

	void                    rebuild() {

		if(item_h) {

			this->pager_instance.set_items_per_page(floor(available_h / (item_h+margin_h)));
		}
	}

	size_t                  available_h, 	//!< Max height for the list.
							item_h,		//!< Height of each single item.
							margin_h;	//!< Vertical margin between items.
};

}

