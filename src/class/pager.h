#pragma once

#include <cstddef>
#include <cmath>

namespace tools{

//!Pagination structure.

//!Keeps current index, page and items_per page values. Due to its static nature
//!it must be reset if the paged data changes. Page data is independent of the
//!instance, which has no access to it.
class pager {
	public:

	//!Creates the pager with the registers per page and and total item count.
							pager(size_t rpp=0, size_t tot=0);

	//!Returns the current page value.
	size_t						get_current_page() const {return current_page;}

	//!Returns the current index value.
	size_t						get_current_index() const {return current_index;}

	//!Returns the current page count value.
	size_t						get_pages_count() const {return pages_count;}

	//!Returns the current total of items.
	size_t						get_item_count() const {return item_count;}

	//!Returns the current items per page value.
	size_t						get_items_per_page() const {return items_per_page;}

	//!Advances-decreases the current page and returns true if the new value 
	//!is legal. Positive values of the first parameter add to the current
	//!page, while negatives decrease it. If the second parameter is set to
	//!false, the current index will not change to reflect a item in the 
	//!new page.
	bool						set_page(int, bool=true);

	//!Selects the next / previous item depending on the +/- sign of the
	//!parameter. Automatically handles page turning.
	bool						set_item(int);

	//!Sets the currently selected index  Does not handle page turning.
	void						set_index(size_t v) {current_index=v;}

	//!Sets the current items per page value.
	void						set_items_per_page(size_t);

	//!Sets the current item count.
	void						set_item_count(size_t);

	//!Resets the current index to zero. Does not handle page turning.
	void						reset_index() {current_index=0;}

	private:

	//!Internally calculates the page count.
	void						calculate_page_info();

	size_t						items_per_page,	//!< Current items per page.
							item_count,	//!< Current quantity of items.
							current_page,	//!< Current page, zero indexed.
							pages_count,	//!< Count of pages.
							current_index;	//!< Current index, zero indexed.
};

}
