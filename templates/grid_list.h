#ifndef TOOLS_GRID_LIST_H
#define TOOLS_GRID_LIST_H

#include "base_list.h"

namespace tools
{

template<typename T>
class grid_list:
	public base_list<T>
{
	public:


	//Estructura que se sirve como parte de un listado.
	struct Item
	{
		size_t 		x, 
				y, 
				index;
		const T& item;
	};

						//Propios...
						grid_list(size_t available_w, size_t available_h, size_t item_w, size_t item_h)
		:available_w(available_w), available_h(available_h),
		item_w(item_w), item_h(item_h),  
		margin_w(0), margin_h(0),
		reg_row(floor(available_w / item_w)),
		reg_colum(floor(available_h / item_h))
	{
		this->pager.set_items_per_page(reg_row * reg_colum);
	}


	const Item				current_line() const 
	{
		size_t index=this->pager.get_current_index();
		size_t rpp=this->pager.get_registers_per_page();
		size_t rel=index % rpp;

		size_t y=floor(rel / reg_row);
		size_t x=floor(rel % reg_row);

		return Item{ (x * item_w) + (x * margin_w), (y * item_h) + (y * margin_h), index, this->get()};
	}


	std::vector<Item>			get_page() const
	{
		std::vector<Item> res;

		size_t rpp=this->pager.get_registers_per_page();
		size_t pa=this->pager.get_current_page();

		auto	ini=std::begin(this->data)+(pa * rpp),
			fin=ini+rpp;

		size_t x=0, y=0, reg=1, index=pa * rpp;

		while(ini < fin && ini < std::end(this->data))
		{
			res.push_back(Item{x, y, index++, *ini});
			++ini;

			if(reg == reg_row)
			{
				x=0;
				reg=1;
				y+=item_h+margin_h;
			}
			else
			{
				++reg;
				x+=item_w+margin_w;
			}
		}

		return res;
	}

	template<typename E>
	bool 					topological_select(size_t rx, size_t ry, E f)
	{
		const auto& pag=get_page();
		for(const auto& itemp : pag)
		{
			if(	rx >= itemp.x
				&& ry >= itemp.y
				&& rx <= itemp.x + item_w
				&& ry <= itemp.y + item_h)
			{
				f(itemp);
				return true;
			}
		}
		return false;
	}	

	//Especializaciones paginación...
	bool					set_row(int v)
	{
		bool result=false;
		for(size_t i=0; i < reg_row ; ++i) result=this->pager.set_item(v) || result;
		return result;
	}

	size_t					get_item_w() const {return item_w;}
	size_t					get_item_h() const {return item_h;}
	size_t					get_margin_w() const {return margin_w;}
	size_t					get_margin_h() const {return margin_h;}
	void					set_margin_w(size_t v) {margin_w=v;}
	void					set_margin_h(size_t v) {margin_h=v;}

	private:

	size_t					available_w, available_h;
	size_t					item_w, item_h;
	size_t					margin_w, margin_h;
	size_t					reg_row, reg_colum;
};

}
#endif
