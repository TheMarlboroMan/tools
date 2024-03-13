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

	enum class dir{next, previous};

	//!Creates the pager with the registers per page and and total item count.
								pager(size_t rpp=0, size_t tot=0);

	//!Returns the current page value.
	size_t						get_current_page() const {return current_page;}

	//!Returns the current index value.
	size_t						get_current_index() const {return current_index;}

	//!Returns the zero-indexed index of the item selected on the current page.
	size_t                      get_relative_index() const {return current_index % items_per_page;}

	//!Returns the current page count value.
	size_t						get_pages_count() const {return pages_count;}

	//!Returns the current total of items.
	size_t						get_item_count() const {return item_count;}

	//!Returns the current items per page value.
	size_t						get_items_per_page() const {return items_per_page;}

	//!Returns true if the last flag setting operation turned the page.
	bool                        is_page_turned() const {return status_flags & page_turned;}

	//!Returns true if the last flag setting operation cycled an item.
	bool                        is_item_cycled() const {return status_flags & item_cycled;}

	//!Advances-decreases the current page. If already in the first-last page,
	//!will try to set the pointer to the first-last item on it. If the second
	//!parameter is set to false, the current index will not change to reflect
	//!a item in the new page. The third parameter is reserved for internal
	//!use. This is a flag setting operation, meaning the the current flags can
	//!change.
	void						turn_page(dir, bool=true, bool=true);

	//!Selects the next / previous item depending. Automatically handles page
	//!turning. This is a flag setting operation, meaning the the current flags
	//!can change.
	void						cycle_item(dir);

	//!Sets the currently selected index  Does not handle page turning not
	//!sets flags.
	void						set_index(size_t v) {

		current_index=v;
	}

	//!Sets the current items per page value.
	void						set_items_per_page(size_t);

	//!Sets the current item count.
	void						set_item_count(size_t);

	//!Resets the current index and page to zero. Does not handle page turning.
	//!Sets flags if values change. Does not reset item count to zero.
	void						reset();

	//!Resets the current index to zero. Does not handle page turning.
	void						reset_index() {current_index=0;}

	private:

	//!Internal flags.
	enum status {page_turned=1, item_cycled=2};

	//!Internally calculates the page count.
	void						calculate_page_info();

	size_t                      items_per_page,	//!< Current items per page.
	                            item_count,	//!< Current quantity of items.
	                            current_page,	//!< Current page, zero indexed.
	                            pages_count,	//!< Count of pages.
	                            current_index;	//!< Current index, zero indexed.
	int                         status_flags{0}; //!< Indicate the result of the previous flag-setting operation.
};

}
