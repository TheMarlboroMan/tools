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
		pages_count=ceil(item_count / items_per_page);
	}
}

bool pager::set_page(int val, bool forzar) {

	if(val > 0) {
		if(current_page < pages_count) {
			++current_page;
			if(forzar) {
				current_index+=items_per_page;
			}
		}
		else if(current_page == pages_count) {
			current_index=item_count-1;
		}

		return true;
	}
	else if(val < 0) {
		if(current_page > 0) {
			--current_page;
			if(forzar) {
				current_index-=items_per_page;
			}
		}
		else {
			current_index=0;
		}

		return true;
	}
	else return false;
}

bool pager::set_item(int val) {
	bool resultado=false;

	if(val > 0 && current_index + 1 < item_count) {
		++current_index;
		resultado=true;
	}
	else if(val < 0 && current_index > 0) {
		--current_index;
		resultado=true;
	}
	
	if(resultado) {
		//Detectar si vamos a cambiar de página... Se hace en dos direcciones...
		size_t indice=current_index % (items_per_page);
		if(val > 0) {
			if(!indice) {
				set_page(val, false);
			}
		}
		else if(val < 0) {
			if(indice==items_per_page - 1) {
				set_page(val, false);
			}
		}
	}

	return resultado;
}
