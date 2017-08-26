#ifndef TOOLS_VIEW_COMPOSER_H
#define TOOLS_VIEW_COMPOSER_H

#include <memory>

#include "dnot_parser.h"
#include <def_video.h>

/**
* El compositor se creó para sacar pantallas sencillas (como intro o game over)
* del código y meterlas en archivos de configuración donde podemos hacer ajustes
* sin necesidad de compilar.
*
* En un principio se trata de un archivo de configuración (en Dnot) por un lado,
* que define las data de las vistas en si. Por otro tenemos que
* conectar esta vista con las fuentes, texturas y superficies del proyecto (esto
* ahora mismo se hace manual... Podríamos hacer una función "mapear" para
* mapear con texturas, superficies y fuentes usando los gestores de turno que
* podríamos pasar por referencia.
*
* Cuando se conecta la vista con los recursos sólo hay que llamar a "parse"
* pasando como primer parámetro el nombre del fichero y como segundo el nombre
* del nodo de la vista.
*
* Una vez parseada la lista la podemos "draw" a una pantalla o podemos extraer
* elementos internos que tengan un "id" para poder manipularlos.
*
* Un manual rápido del formato del fichero lo ponemos al final de este mismo
* archivo.
*/

namespace tools
{
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
	static const std::string		texture_key;
	static const std::string		surface_key;
	static const std::string		font_key;
	static const std::string		brush_key;
	static const std::string		visible_key;
	static const std::string		external_key;
	static const std::string		external_reference_key;
	static const std::string		rotation_key;

	struct position{int x, y;};

	//TODO: Comentar...

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

/*
* El fichero de configuración debe tener tantos nodos base como escenas 
* configura.
* En nuestro caso, el nodo base de la escena se llama "escena_prueba". El nodo 
* base debe ser una lista de objetos, cada uno identificado por un "tipo". Todos
* los objetos pueden tener la clave "order" y "alpha" para configurar el order
* de aparición (menor a mayor) e "id" para sacar referencias al código.
*
* Los tipos posibles son:
* - pantalla:
*	rgba[r, g, b, a] : color con el que se coloreará la pantalla.
* - caja:
*	pos[x, y, w, h] : posición de la caja
*	rgb[r, g, b] : color de la caja
* - bitmap:
*	pos[x, y, w, h] : posición de la caja
*	rec[x, y, w, h] : recorte de la textura
*	textura["cadena"] : cadena de mapeo de la textura
* INIT DEPRECATED DEPRECATED DEPRECATED DEPRECATED DEPRECATED DEPRECATED DEPRECATED 
* - patron:
*	pos[x, y, w, h] : posición de la caja
*	rec[x, y, w, h] : recorte de la textura
*	textura["cadena"] : cadena de mapeo de la textura
*	pincel:[x, y, w, h] : rectángulo de pincel que se usará para dibujar
* - ttf:
*	pos[x, y] : posición del texto
*	fuente["cadena"] : cadena de mapeo de la fuente TTF
*	texto["texto"] : Texto que se dibujará
*	rgba[r, g, b, a] : color de la fuente
*
* El fichero viene a quedar así.
*
escena_prueba:
[
	{
		tipo:"pantalla", 
		rgba:[32, 32, 32, 255]
	},
	{
		tipo:"caja", id:"mi_caja", 
		pos:[20,20,80,80], rgb:[255, 0, 0],
		order:1
	},
	{
		tipo:"caja",
		pos:[80, 16, 40, 40], rgb:[0, 0, 255],
		order:2
	},
	{
		tipo:"bitmap",
		pos:[200, 200, 14, 30], rec:[0, 0, 14, 30],
		textura:"sprites", alpha:192
	},
	{
		tipo:"ttf",
		pos:[20, 180],
		fuente:"fuente",
		texto:"Hola... Yo también salgo de un fichero",
		rgba:[64, 64, 255, 128]
	}
]
*/

#endif
