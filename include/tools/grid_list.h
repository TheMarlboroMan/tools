#pragma once
#include "base_list.h"
#include <functional>

namespace tools{

//!A list ordered in a visual grid.
template<typename T>
class grid_list:
	public base_list<T> {
	public:

	//!Each item of the list: encapsulates position, index and represented value.
	struct Item {
		std::size_t             x, 	//!< X position.
		                        y, 	//!< Y position.
								index;	//!< Zero-based index.
		const T&                item;//!< Represented value.
	};

		//!Class constructor. Takes the available size and item size.
	                            grid_list(
			std::size_t available_w,
			std::size_t available_h,
			std::size_t item_w,
			std::size_t item_h
		)
			:available_w(available_w),
			available_h(available_h),
			item_w(item_w),
			item_h(item_h),
			margin_w(0),
			margin_h(0),
			reg_row{floor(available_w / (item_w+margin_w) )},
			reg_column{floor(available_h / (item_h+margin_h))} {
		this->pager_instance.set_items_per_page(reg_row * reg_column);
	}

	//!Selects the next item...
	void                        next_row() {

		for(int i=0; i<reg_row; i++) {
			this->next();
		}
	}

	//!Selects the next item...
	void                        previous_row() {

		for(int i=0; i<reg_row; i++) {
			this->previous();
		}
	}

	//!Seems that it returns the currently selected item. TODO: Where is this used? What does it do?.
	const Item                  current_line() const {

		std::size_t	index=this->pager_instance.get_current_index(),
			rpp=this->pager_instance.get_items_per_page(),
			rel=index % rpp;

		std::size_t	y=floor(rel / reg_row),
			x=floor(rel % reg_row);

		return Item{ (x * item_w) + (x * margin_w), (y * item_h) + (y * margin_h), index, this->get()};
	}


	//!Returns a vector of items that represent the current page, as per
	//!the internal pager class.
	std::vector<Item>           get_page() const {

		std::vector<Item> res;

		std::size_t rpp=this->pager_instance.get_items_per_page(),
			pa=this->pager_instance.get_current_page();

		auto	ini=std::begin(this->data)+(pa * rpp),
			fin=ini+rpp;

		std::size_t x=0, y=0, reg=1, index=pa * rpp;

		while(ini < fin && ini < std::end(this->data)) {

			res.push_back({x, y, index++, *ini});
			++ini;

			if(reg == reg_row) {
				x=0;
				reg=1;
				y+=item_h+margin_h;
			}
			else {
				++reg;
				x+=item_w+margin_w;
			}
		}

		return res;
	}

	//!Applies f to the element in the given position. Returns true if there
	//!was an element in the position, false if not.
	template<typename E>
	bool                    topological_select(
		std::size_t rx,
		std::size_t ry,
		E f
	) {
		const auto& pag=get_page();
		for(const auto& itemp : pag) {
			if(	rx >= itemp.x
				&& ry >= itemp.y
				&& rx <= itemp.x + item_w
				&& ry <= itemp.y + item_h) {
				f(itemp);
				return true;
			}
		}
		return false;
	}

	//!Seems to return true if the current page contains the item v.
	//TODO: I really don't know what this does.
	bool                    set_row(int v) {
		bool result=false;
		for(std::size_t i=0; i < reg_row ; ++i) {
			result=this->pager_instance.set_item(v) || result;
		}
		return result;
	}

	//!Returns the fixed width for each item.
	std::size_t             get_item_w() const {return item_w;}

	//!Returns the fixed height for each item.
	std::size_t             get_item_h() const {return item_h;}

	//!Returns the horizontal margin between items.
	std::size_t             get_margin_w() const {return margin_w;}

	//!Returns the vertical margin between items.
	std::size_t             get_margin_h() const {return margin_h;}

	//!Returns the available width.
	std::size_t             get_available_w() const {return available_w;}

	//!Returns the available height.
	std::size_t             get_available_h() const {return available_h;}

	//!Sets the width of each item.
	grid_list<T>&           set_item_w(std::size_t v) {

		item_w=v;
		this->recalculate();
		return *this;
	}

	//!Sets the height of each item.
	grid_list<T>&           set_item_h(std::size_t v) {

		item_h=v;
		this->recalculate();
		return *this;
	}

	//!Sets the available horizontal spacing
	grid_list<T>&           set_available_w(std::size_t v) {

		available_w=v;
		this->recalculate();
		return *this;
	}

	//!Sets the available vertical spacing
	grid_list<T>&           set_available_h(std::size_t v) {

		available_h=v;
		this->recalculate();
		return *this;
	}

	//!Sets the horizontal margin between items.
	grid_list<T>&           set_margin_w(std::size_t v) {

		margin_w=v;
		this->recalculate();
		return *this;
	}

	//!Sets the vertical margin between items.
	grid_list<T>&           set_margin_h(std::size_t v) {

		margin_h=v;
		this->recalculate();
		return *this;
	}

	private:

	void                   recalculate() {

		reg_row=floor(available_w / (item_w+margin_w));
		reg_column=floor(available_h / (item_h+margin_h));
	}

	std::size_t				available_w,	//!< Available width the for list.
						available_h,	//!< Available height the for list.
						item_w, 	//!< Fixed width for each item.
						item_h,		//!< Fixed height for each item.
						margin_w, 	//!< Fixed horizontal distance between items.
						margin_h,	//!< Fixed vertical distance between items.
						reg_row, 	//!< Max items per row.
						reg_column;	//!< Max items per column.
};

}
