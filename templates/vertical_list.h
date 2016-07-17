#ifndef TOOLS_VERTICAL_LIST_H
#define TOOLS_VERTICAL_LIST_H

#include "base_list.h"

namespace tools
{

template<typename T>
class vertical_list:
	public base_list<T>
{
	public:


	//Estructura que se sirve como parte de un listado.
	struct Item
	{
		size_t 		y, 
				index;
		const T& 	item;
	};

						//Propios...
						vertical_list(size_t available_h, size_t item_h)
		:available_h(available_h), item_h(item_h), margin_h(0)
	{
		this->pager_instance.set_items_per_page(floor(available_h / item_h));
	}

	const Item				current_line() const 
	{
		size_t i_actual=this->pager_instance.get_current_index();
		size_t index=i_actual % this->pager_instance.get_items_per_page();

		return Item{ 
			(item_h * index) + (margin_h * index),  
			i_actual,
			this->get()
		};
	}

	const Item				line(size_t i) const 
	{
		return Item{(item_h * i) + (margin_h * i), 
				this->pager_instance.get_current_index(), 
				this->data[i]};
	}

	std::vector<Item>			get_page() const
	{
		std::vector<Item> res;

		size_t rpp=this->pager_instance.get_items_per_page();
		size_t pa=this->pager_instance.get_current_page();

		auto	ini=std::begin(this->data)+(pa * rpp),
			fin=ini+rpp;

		size_t y=0, index=pa*rpp;

		while(ini < fin && ini < std::end(this->data))
		{
			res.push_back(Item{y, index++, *ini});
			++ini;
			y+=this->item_h+margin_h;
		}

		return res;
	}

	template<typename E> 
	bool 					topological_select(size_t ry, E f)
	{
		const auto& pag=get_page();
		for(const auto& itemp : pag)
		{			
			if(ry >= itemp.y && ry <= itemp.y + this->item_h)
			{
				f(itemp);
				return true;
			}
		}
		return false;
	}

	size_t					get_item_h() const	{return item_h;}
	size_t					get_margin_h() const	{return margin_h;}
	void					set_margin_h(size_t v) {margin_h=v;}
	void					set_available_h(size_t v) 
	{
		available_h=v;
		if(item_h) this->pager_instance.set_items_per_page(floor(available_h / item_h));
	}
	void					set_item_h(size_t v) 
	{
		item_h=v;
		this->pager_instance.set_items_per_page(floor(available_h / item_h));
	}

	private:

	size_t					available_h,
						item_h,
						margin_h;
};

}
#endif
