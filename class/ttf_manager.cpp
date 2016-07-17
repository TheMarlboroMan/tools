#include "ttf_manager.h"

#include <stdexcept>

using namespace tools;

const ldv::ttf_font& ttf_manager::get(const std::string& f, int t) const
{
	if(!data.count({f, t}))
	{
		throw std::runtime_error("TTF font "+f+" was not registered in the requested size");
	}

	return data.at({f,t});
}

void ttf_manager::insert(const std::string& f, int t, const std::string& r)
{
	if(!data.count({f, t}))
	{
		data.insert( std::pair<font_info, ldv::ttf_font>({f, t}, ldv::ttf_font(r, t) ) );
	}
}
