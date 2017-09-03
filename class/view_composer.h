#ifndef TOOLS_VIEW_COMPOSER_H
#define TOOLS_VIEW_COMPOSER_H

#include <memory>

#include "dnot_parser.h"
#include <def_video.h>


namespace tools
{

//!The view composer creates graphical presentations through configuration files with little coding and compiling.
/**
It works in two stages: a dnot configuration file which defines the elements
to be drawn and a code snippet to connect textures and fonts to it. Once the 
file is done and the resources are connected to the composer, a call to 
"parse" with the filename and the view node (dnots can have many root nodes)
will mount the view and the call to "draw" will draw it.

Elements drawn can be internal or external. Internal elements have an id that
can be used in code to manipulate them. External elements exist in code and 
are made to correspond with a key in the file.

The structure of the file goes like this:

layout_id:[
	{type:"#type_key#", #attribute#: #attribute_value#},
	{type:"#type_key#", #attribute#: #attribute_value#},
	{type:"#type_key#", #attribute#: #attribute_value#},
	{type:"#type_key#", #attribute#: #attribute_value#}
]

other_layout_id:[
	...
]

External features like textures or fonts need to be linked and given a string
handle through the methods map_texture(handle, texture), map_surface(handle, surface)
or map_font(handle, font). The handle will be used within the file to represent
the resource. These links or "maps" need to be done before "parse" is called!.

In the file, "type" corresponds either to a definition or a representation. 
Definitions can be integer or float values that can be used outside the 
composer (that is, in the code, through the get_int or get_float values) to
avoid recompilation. So far, considering how the file is parsed as a regular
dnot, there is no support for internal definitions.
Definitions are expressed like this:

	{type:"define", key:"my_key", value:32}

As for representations, there are common attributes:

	type: expresses the type as a string
	id: string handle used to manipulate the representation from the code
	alpha: integer alpha value. By default, all representations are set to blend_mode alpha.
	order: integer order value. Lower values are drawn sooner.
	visible: boolean value, indicates whether the representation is drawn or not.
	rotate: [30, 40, 40] integer list corresponding to degrees, rotation center x and y.

Different representations and their parameters are

screen:
	rgba:[255,255,255,255] (Screen color fill)
	does not accept the "order" attribute.

ttf:
	location:[16, 16] 	(position)
	text:"this is the text",(text)
	font:"font handle", 	(font handle)
	rgba:[0, 0, 0, 255],	(font color)

external:
	ref:"menu"	(allows a code representation to be included in the view
			and use the "order" attribute. register_as_external
			is used for this purpose).

bitmap:
	location:[140, 300, 546, 376], 	(position)
	clip:[0, 0, 546, 376],		(texture clip)
	brush:[20,20],			(draw brush size)
	texture:"texture handle"	(texture handle)
	surface:"surface handle"	(surface handle... untested).

box:
	location:[0, 0, 800, 600]		(position)
	rgba:[255,255,255,255]		(color)

polygon:
	points:[[0,0], [10,10], [20,10]]	(points)
	rgba:[255,255,255,255]			(color)
	fill: "fill"|"line"				(type of fill)
*/

class view_composer
{
	public:

	typedef std::unique_ptr<ldv::representation> uptr_rep;

				view_composer();
	void			parse(const std::string&, const std::string&); //Puede tirar std::runtime_error.
	void			draw(ldv::screen&);
	void			draw(ldv::screen&, const ldv::camera&);
	void			map_texture(const std::string&, ldv::texture *);
	void			map_texture(const std::string&, ldv::texture&);
	void			map_surface(const std::string&, ldv::surface *);
	void			map_surface(const std::string&, ldv::surface&);
	void			map_font(const std::string&, const ldv::ttf_font *);
	void			map_font(const std::string&, const ldv::ttf_font&);
	void			clear_view();
	void			clear_definitions();

	//!Empties the representation, allowing for a new call to "parse".
	void			clear()
	{
		clear_view();
		clear_definitions();
	}
	ldv::representation * 	get_by_id(const std::string&);
	bool			id_exists(const std::string&) const;
	int			get_int(const std::string&) const;
	float			get_float(const std::string&) const;
	void			register_as_external(const std::string&, ldv::representation&);

	private:

	//!Internal template helper to get definitions.
	template<typename T> 
	T get_definition(const std::string k, const std::map<std::string, T>& map) const
	{
		try
		{
			return map.at(k);
		}
		catch(std::exception& e)
		{
			throw std::runtime_error("unable to locate definition of "+k);
		}
	}

	static const std::string		type_key;
	static const std::string		box_key;
	static const std::string		bitmap_key;
	static const std::string		text_key;
	static const std::string		ttf_key;
	static const std::string		polygon_key;
	static const std::string		screen_key;
	static const std::string		definition_key;
	static const std::string		definition_key_key;
	static const std::string		definition_key_value;
	static const std::string		order_key;
	static const std::string		alpha_key;
	static const std::string		id_key;
	static const std::string		rgba_key;
	static const std::string		location_key;
	static const std::string		clip_key;
	static const std::string		points_key;
	static const std::string		polygon_fill_key;
	static const std::string		texture_key;
	static const std::string		surface_key;
	static const std::string		font_key;
	static const std::string		brush_key;
	static const std::string		visible_key;
	static const std::string		external_key;
	static const std::string		external_reference_key;
	static const std::string		rotation_key;

	struct position{int x, y;};

	//!Represents a singular drawable.

	struct item
	{
		uptr_rep			rep;
		ldv::representation *		ptr;
		int 				order;

		item(uptr_rep&& pr, int porder=0)
			:rep(std::move(pr)), ptr(rep.get()), order(porder)
		{

		}

		item(ldv::representation * p, int porder=0)
			:rep(nullptr), ptr(p), order(porder)
		{}

		bool operator<(const item& o) const
		{
			return order < o.order;
		}

		void draw(ldv::screen& p)
		{
			ptr->draw(p);
		}

		void draw(ldv::screen& p, const ldv::camera& cam)
		{
			ptr->draw(p, cam);
		}
	};


	uptr_rep		create_box(const dnot_token&);
	uptr_rep		create_bitmap(const dnot_token&);
	uptr_rep		create_ttf(const dnot_token&);
	uptr_rep		create_polygon(const dnot_token&);
	void			do_screen(const dnot_token&);
	void			do_definition(const dnot_token&);

	ldv::rect		box_from_list(const dnot_token&);
	ldv::rgba_color		rgba_from_list(const dnot_token&);
	position		position_from_list(const dnot_token&);

	std::vector<item>				data;
	std::map<std::string, ldv::representation*>	id_map;
	std::map<std::string, ldv::representation*>	external_map;
	std::map<std::string, ldv::texture*>		texture_map;
	std::map<std::string, ldv::surface*>		surface_map;
	std::map<std::string, const ldv::ttf_font*>	font_map;
	std::map<std::string, int>			int_definitions;
	std::map<std::string, float>			float_definitions;

	bool 						with_screen;
	ldv::rgba_color					screen_color;
};
}

#endif
