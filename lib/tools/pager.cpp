#include <tools/pager.h>

#include <iostream>

using namespace tools;

pager::pager(size_t rpp, size_t pitem_count)
	:items_per_page(rpp), item_count(pitem_count), current_page(0),
	pages_count(0), current_index(0) {

	//Prevenir una división por 0...
	if(items_per_page && item_count) {

		calculate_page_info();
	}
}

void pager::set_item_count(size_t val) {
	item_count=val;
	calculate_page_info();
}

void pager::set_items_per_page(size_t val) {
	items_per_page=val;
	calculate_page_info();
}

void pager::calculate_page_info() {

	if(items_per_page) {

		pages_count=ceil((double)item_count / (double)items_per_page);
	}
}

void pager::turn_page(pager::dir _dir, bool forzar, bool _clear_flags) {

	if(_clear_flags) {
		status_flags=0;
	}

	if(_dir==dir::next) {

		if(current_page < pages_count-1) {

			++current_page;
			status_flags|=status::page_turned;

			if(forzar) {

				current_index+=items_per_page;

				if(current_index>=item_count) {
					current_index=item_count-1;
				}
				status_flags|=status::item_cycled;
			}
		}
		//Try to set the last item...
		else if(current_index!=item_count-1) {

			current_index=item_count-1;
			status_flags|=status::item_cycled;
		}
	}
	else {

		if(current_page > 0) {

			--current_page;
			status_flags|=status::page_turned;

			if(forzar) {

				current_index-=items_per_page;
				status_flags|=status::item_cycled;
			}
		}
		//Try to set the last item...
		else if(0!=current_index) {

			current_index=0;
			status_flags|=status::item_cycled;
		}
	}
}

void pager::cycle_item(pager::dir _val) {

	status_flags=0;

	if(_val==dir::next) {

		if(current_index+1 >= item_count) {
			return;
		}

		++current_index;
	}
	else {

		if(0==current_index) {
			return;
		}

		--current_index;
	}

	status_flags|=status::item_cycled;

	//Detectar si vamos a cambiar de página... Se hace en dos direcciones...
	size_t indice=current_index % items_per_page;
	if(_val==dir::next) {
		if(!indice) {
			turn_page(_val, false, false);
		}
	}
	else {
		if(indice==items_per_page - 1) {
			turn_page(_val, false, false);
		}
	}
}

void pager::reset() {

	status_flags=0;

	if(0!=current_page) {
		status_flags|=status::page_turned;
		current_page=0;
	}

	if(0!=current_index) {
		status_flags|=status::item_cycled;
		current_index=0;
	}
}

void pager::set_index(
	std::size_t _index,
	bool _handle_side_effects
) {

	if(_handle_side_effects && current_index != _index) {

		status_flags|=status::item_cycled;
	}

	if(!_handle_side_effects) {

		current_index=_index;
		return;
	}

	//This handles the side effects.
	auto old_page=floor(current_index / items_per_page);

	current_index=_index;
	current_page=floor(current_index / items_per_page);
	if(old_page != current_page) {

		status_flags|=status::page_turned;
	}
}
