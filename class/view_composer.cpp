#include "view_composer.h"

#include <algorithm>

using namespace tools;

const std::string view_composer::type_key="type";
const std::string view_composer::box_key="box";
const std::string view_composer::bitmap_key="bitmap";
const std::string view_composer::ttf_key="ttf";
const std::string view_composer::polygon_key="polygon";
const std::string view_composer::screen_key="screen";
const std::string view_composer::definition_key="define";
const std::string view_composer::definition_key_key="key";
const std::string view_composer::definition_key_value="value";
const std::string view_composer::alpha_key="alpha";
const std::string view_composer::order_key="order";
const std::string view_composer::id_key="id";
const std::string view_composer::rgba_key="rgba";
const std::string view_composer::location_key="location";
const std::string view_composer::clip_key="clip";
const std::string view_composer::points_key="points";
const std::string view_composer::polygon_fill_key="fill";
const std::string view_composer::text_key="text";
const std::string view_composer::surface_key="surface";
const std::string view_composer::font_key="font";
const std::string view_composer::texture_key="texture";
const std::string view_composer::brush_key="brush";
const std::string view_composer::visible_key="visible";
const std::string view_composer::external_key="external";
const std::string view_composer::external_reference_key="ref";
const std::string view_composer::rotation_key="rotate";

//!Default constructor.

view_composer::view_composer()
	:with_screen(false), screen_color{0,0,0, 255}
{

}

//!Draws the composition to the screen.

void view_composer::draw(ldv::screen& p)
{
	if(with_screen)
	{
		p.clear(ldv::rgba8(screen_color.r, screen_color.g, screen_color.b, screen_color.a));
	}
	
	for(auto& r : data)
	{
		r.draw(p);
	}
}

//!Draws the composition to the screen using a camera.

void view_composer::draw(ldv::screen& p, const ldv::camera& cam)
{
	if(with_screen)
	{
		p.clear(ldv::rgba8(screen_color.r, screen_color.g, screen_color.b, screen_color.a));
	}
	
	for(auto& r : data)
	{
		r.draw(p, cam);
	}
}

//!Maps the texture to the given handle.

//!Mapped textures can be referenced in the layout file by their handle. Textures
//!are not copied, so it is required that they exist as long as the layout
//!needs to be drawn. Will throw if the key is used more than once.

void view_composer::map_texture(const std::string& clave, ldv::texture * tex)
{
	if(texture_map.count(clave))
	{
		throw std::runtime_error("Repeated key for texture map "+clave);
	}
	
	texture_map[clave]=tex;
}

//!Maps the texture.

void view_composer::map_texture(const std::string& clave, ldv::texture& tex)
{
	map_texture(clave, &tex);
}

//!Same as map textures, but with surfaces.

void view_composer::map_surface(const std::string& clave, ldv::surface * sup)
{
	if(surface_map.count(clave))
	{
		throw std::runtime_error("Repeated key for surface map "+clave);
	}
	
	surface_map[clave]=sup;
}

//!Same as map textures, but with surfaces.

void view_composer::map_surface(const std::string& clave, ldv::surface& sup)
{
	map_surface(clave, &sup);
}

//!Same as map textures, but with ttf_fonts.

void view_composer::map_font(const std::string& clave, const ldv::ttf_font * fuente)
{
	if(font_map.count(clave))
	{
		throw std::runtime_error("Repeated key for font map "+clave);
	}
	
	font_map[clave]=fuente;
}

//!Same as map textures, but with ttf_fonts.

void view_composer::map_font(const std::string& clave, const ldv::ttf_font& fuente)
{
	map_font(clave, &fuente);
}

//!Returns the representation with the given id.

//!Will throw if there is no representation with that id. The representation
//!is returned as a pointer to its most base type (ldv::representation *)
//!so manipulation of specific characteristics (like textures or texts) need
//!to be done by casting the pointer (static_cast<ldv::bitmap_representation *>)
//!to the necessary type.

ldv::representation * view_composer::get_by_id(const std::string& id)
{
	try
	{
		return id_map.at(id);
	}
	catch(std::exception& e)
	{
		throw std::runtime_error("Unable to locate element with id "+id+". Is the view mounted?");
	}
}

//!Checks if there exists a representation with the given id.

bool view_composer::id_exists(const std::string& id) const
{
	return id_map.count(id);
}

//!Parses the file and mounts the layout identified by the second parameter.

//!Will throw if the file or node does not exists, or if there are malformations
//!in the layout file.

void view_composer::parse(const std::string& ruta, const std::string& nodo)
{
	dnot_token tok=tools::dnot_parse(ruta);
	const auto& lista=tok[nodo].get_vector();

	for(const auto& token : lista)
	{
		uptr_rep ptr;
		const auto& tipo=token[type_key].get_string(); //Si no hay tipo vamos a explotar. Correcto.
		int order=0;

		if(tipo==box_key) ptr=std::move(create_box(token));
		else if(tipo==bitmap_key) ptr=std::move(create_bitmap(token));
		else if(tipo==ttf_key) ptr=std::move(create_ttf(token));
		else if(tipo==polygon_key) ptr=std::move(create_polygon(token));
		else if(tipo==external_key)
		{
			const std::string& ref=token[external_reference_key];
			if(!external_map.count(ref))
			{
				throw std::runtime_error("Key for '"+ref+"' has not been externally registered.");
			}

			if(token.key_exists(order_key)) 
			{
				order=token[order_key];
			}

			data.push_back(item(external_map[ref], order));
			continue;
		}
		else if(tipo==screen_key) 
		{
			do_screen(token);
			continue;
		}
		else if(tipo==definition_key)
		{
			do_definition(token);
			continue;
		}
		else
		{
			throw std::runtime_error("Unknown '"+tipo+"' when parsing view");
		}

		//Tratamiento de cosas comunes...
		if(token.key_exists(order_key)) 
		{
			order=token[order_key];
		}

		//Tratamiento de cosas comunes...
		if(token.key_exists(alpha_key))
		{
			ptr->set_blend(ldv::representation::blends::alpha);
			ptr->set_alpha((Uint8)token[alpha_key].get_int());
		}

		if(token.key_exists(rotation_key))
		{
			auto values=token[rotation_key].get_vector();
			if(values.size()!=3) throw std::runtime_error("Rotate needs three parameters");

			auto * rrot=static_cast<ldv::representation *>(ptr.get());
			rrot->set_rotation((int)values[0]);
			rrot->set_rotation_center((int)values[1], (int)values[2]);
		}

		if(token.key_exists(visible_key))
		{
			ptr->set_visible(token[visible_key].get_bool());
		}

		if(token.key_exists(id_key))
		{
			const std::string& id=token[id_key];
			if(id_map.count(id))
			{
				throw std::runtime_error("Repeated id key '"+token[id_key].get_string()+"' for view");
			}
			else
			{
				id_map[id]=ptr.get();
			}
		}

		//Y finalmente insertamos.
		data.push_back(item(std::move(ptr), order));
	}

	std::sort(std::begin(data), std::end(data));
}

//!Registers the given representation with the handle.

//!Registered representations can use the values expressed in the layout file.
//!Will throw if the reference is repeated.

void view_composer::register_as_external(const std::string& clave, ldv::representation& rep)
{
	if(external_map.count(clave))
	{
		throw std::runtime_error("Repeated key "+clave+" for external representation");
	}
	
	external_map[clave]=&rep;
} 

//!Creates a box from a token. Internal.
view_composer::uptr_rep view_composer::create_box(const dnot_token& token)
{
	auto color=rgba_from_list(token[rgba_key]);
	uptr_rep res(new ldv::box_representation(ldv::polygon_representation::type::fill, box_from_list(token[location_key]), color));
	res->set_blend(ldv::representation::blends::alpha);
	return res;
}

//!Creates a polygon from a token. Internal.
view_composer::uptr_rep view_composer::create_polygon(const dnot_token& token)
{
	auto color=rgba_from_list(token[rgba_key]);

	ldv::polygon_representation::type t=ldv::polygon_representation::type::fill;
	std::string sfill=token[polygon_fill_key];
	if(sfill=="fill")		t=ldv::polygon_representation::type::fill;
	else if(sfill=="line")		t=ldv::polygon_representation::type::line;
	else throw std::runtime_error("Invalid fill type for polygon");
	
	std::vector<ldv::point>	vp;
	for(const auto& l : token[points_key].get_vector()) vp.push_back({l[0], l[1]});

	uptr_rep res(new ldv::polygon_representation(t, vp, color));
	res->set_blend(ldv::representation::blends::alpha);
	return res;
}

//!Creates a bitmap from a token. Internal.
view_composer::uptr_rep view_composer::create_bitmap(const dnot_token& token)
{
	if(!texture_map.count(token[texture_key]))
	{
		throw std::runtime_error("Unable to locate texture "+token[texture_key].get_string()+" for bitmap");
	}

	uptr_rep res(new ldv::bitmap_representation(*texture_map[token[texture_key]], box_from_list(token[location_key]), box_from_list(token[clip_key])));

	if(token.key_exists(brush_key))
	{		
		static_cast<ldv::bitmap_representation *>(res.get())->set_brush(token[brush_key][0], token[brush_key][1]);
	}

	return res;
}

//!Creates a ttf representation from a token. Internal.
view_composer::uptr_rep view_composer::create_ttf(const dnot_token& token)
{
	if(!font_map.count(token[font_key]))
	{
		throw std::runtime_error("Unable to locate font "+token[font_key].get_string()+" for ttf");
	}

	uptr_rep res(new ldv::ttf_representation(*font_map[token[font_key]], rgba_from_list(token[rgba_key]), token[text_key].get_string()));
	res->set_blend(ldv::representation::blends::alpha);

	auto pos=position_from_list(token[location_key]);
	res->go_to({pos.x, pos.y});
	return res;
}

//!Records screen color fill values. Internal.
void view_composer::do_screen(const dnot_token& token)
{
	screen_color=rgba_from_list(token[rgba_key]);
	with_screen=true;
}

//!Records a definition. Internal.
void view_composer::do_definition(const dnot_token& token)
{
	const std::string& clave=token[definition_key_key].get_string();

	if(int_definitions.count(clave)) throw std::runtime_error("Repeated definition for "+clave);

	if(token[definition_key_value].is_int_value()) int_definitions[clave]=token[definition_key_value].get_int();
	else if(token[definition_key_value].is_float_value()) float_definitions[clave]=token[definition_key_value].get_float();
	else throw std::runtime_error("Invalid data type for definition.");
}

//!Creates a box from a token. Internal.
ldv::rect view_composer::box_from_list(const dnot_token& tok)
{
	int x=tok[0], y=tok[1], w=tok[2], h=tok[3];
	return ldv::rect{x, y, (unsigned int)w, (unsigned int)h};
}

//!Creates a rgba from a token. Internal.
ldv::rgba_color view_composer::rgba_from_list(const dnot_token& tok)
{
	try
	{
		int r=tok[0], g=tok[1], b=tok[2], a=tok[3];
		return ldv::rgba8(r, g, b, a);
	}
	catch(std::exception& e)
	{
		throw std::runtime_error(std::string("Unable to parse rgba value... did you forget to add the alpha? : ")+e.what());
	}
}

//!Creates a position box from a token. Internal.
view_composer::position view_composer::position_from_list(const dnot_token& tok)
{
	int x=tok[0], y=tok[1];
	return position{x, y};
}

//!Clears all view elements.

//!Representations, id maps and external references are cleared. Definitions 
//!are not.
void view_composer::clear_view()
{
	data.clear();
	id_map.clear();
	external_map.clear();
}

//!Clears all definitions.
void view_composer::clear_definitions()
{
	int_definitions.clear();
	float_definitions.clear();
}

//!Gets a defined integer. 

//!Will throw if the definition does not exist.
int view_composer::get_int(const std::string& k) const
{
	return get_definition(k, int_definitions);
}

//!Gets a defined float. 

//!Will throw if the definition does not exist.
float view_composer::get_float(const std::string& k) const
{
	return get_definition(k, float_definitions);
}
