#ifndef TOOLS_PAGER_H
#define TOOLS_PAGER_H

/**
* Estructura de paginación que incluye un índice para seleccionar algo.
*/

#include <cstddef>
#include <cmath>

namespace tools
{

class pager
{
	public:
							pager(size_t rpp=0, size_t tot=0);

	size_t						get_current_page() const {return current_page;}
	size_t						get_current_index() const {return current_index;}
	size_t						get_pages_count() const {return pages_count;}
	size_t						get_item_count() const {return item_count;}
	size_t						get_items_per_page() const {return items_per_page;}

	bool						set_page(int, bool=true);
	bool						set_item(int);
	void						set_index(size_t v) {current_index=v;}	
	void						set_items_per_page(size_t);
	void						set_item_count(size_t);
	void						reset_index() {current_index=0;}

	private:

	void						calculate_page_info();

	size_t						items_per_page,
							item_count,
							current_page,
							pages_count,
							current_index;
};

}
#endif
