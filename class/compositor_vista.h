#ifndef COMPOSITOR_VISTA_H
#define COMPOSITOR_VISTA_H

#include <memory>
#include <algorithm>

#include "dnot_parser.h"
#include <video/pantalla/pantalla.h>
#include <video/camara/camara.h>
#include <video/representacion/representacion.h>
#include <video/representacion/representacion_primitiva/representacion_primitiva_caja/representacion_primitiva_caja.h>
#include <video/representacion/representacion_grafica/representacion_bitmap/representacion_bitmap.h>
#include <video/representacion/representacion_grafica/representacion_bitmap/representacion_bitmap_patron.h>
#include <video/representacion/representacion_grafica/representacion_texto/representacion_texto_auto.h>
#include <video/representacion/representacion_grafica/representacion_ttf/representacion_ttf.h>

/**
* El compositor se creó para sacar pantallas sencillas (como intro o game over)
* del código y meterlas en archivos de configuración donde podemos hacer ajustes
* sin necesidad de compilar.
*
* En un principio se trata de un archivo de configuración (en Dnot) por un lado,
* que define las representaciones de las vistas en si. Por otro tenemos que
* conectar esta vista con las fuentes, texturas y superficies del proyecto (esto
* ahora mismo se hace manual... Podríamos hacer una función "mapear" para
* mapear con texturas, superficies y fuentes usando los gestores de turno que
* podríamos pasar por referencia.
*
* Cuando se conecta la vista con los recursos sólo hay que llamar a "parsear"
* pasando como primer parámetro el nombre del fichero y como segundo el nombre
* del nodo de la vista.
*
* Una vez parseada la lista la podemos "volcar" a una pantalla o podemos extraer
* elementos internos que tengan un "id" para poder manipularlos.
*
* Un manual rápido del formato del fichero lo ponemos al final de este mismo
* archivo.
*/

namespace Herramientas_proyecto
{
class Compositor_vista
{
	public:

	typedef std::unique_ptr<DLibV::Representacion> uptr_representacion;

				Compositor_vista();
	void			parsear(const std::string&, const std::string&); //Puede tirar std::runtime_error.
	void			volcar(DLibV::Pantalla&);
	void			volcar(DLibV::Pantalla&, const DLibV::Camara&);
	void			mapear_textura(const std::string&, DLibV::Textura *);
	void			mapear_textura(const std::string&, DLibV::Textura&);
	void			mapear_superficie(const std::string&, DLibV::Superficie *);
	void			mapear_superficie(const std::string&, DLibV::Superficie&);
	void			mapear_fuente(const std::string&, const DLibV::Fuente_TTF *);
	void			mapear_fuente(const std::string&, const DLibV::Fuente_TTF&);
	void			vaciar_vista();
	void			vaciar_constantes();
	void			vaciar()
	{
		vaciar_vista();
		vaciar_constantes();
	}
	DLibV::Representacion * obtener_por_id(const std::string&);
	bool			existe_id(const std::string&) const;
	int			const_int(const std::string&) const;
	float			const_float(const std::string&) const;
	void			registrar_externa(const std::string&, DLibV::Representacion&);

	private:

	template<typename T> 
	T obtener_const(const std::string k, const std::map<std::string, T>& map) const
	{
		try
		{
			return map.at(k);
		}
		catch(std::exception& e)
		{
			throw std::runtime_error("No se localiza la clave de constante "+k);
		}
	}

	static const std::string		clave_tipo;
	static const std::string		clave_caja;
	static const std::string		clave_bitmap;
	static const std::string		clave_texto;
	static const std::string		clave_patron;
	static const std::string		clave_ttf;
	static const std::string		clave_pantalla;
	static const std::string		clave_constante;
	static const std::string		clave_orden;
	static const std::string		clave_alpha;
	static const std::string		clave_id;
	static const std::string		clave_rgb;
	static const std::string		clave_rgba;
	static const std::string		clave_pos;
	static const std::string		clave_rec;
	static const std::string		clave_textura;
	static const std::string		clave_superficie;
	static const std::string		clave_fuente;
	static const std::string		clave_estatica;
	static const std::string		clave_pincel;
	static const std::string		clave_visibilidad;
	static const std::string		clave_externa;
	static const std::string		clave_ref_externa;
	static const std::string		clave_rotacion;

	struct color
	{
		int r, g, b;
	};

	struct color_rgba
	{
		int r, g, b, a;
	};

	struct posicion
	{
		int x, y;
	};

	//TODO: Comentar...

	struct item
	{
		uptr_representacion		rep;
		DLibV::Representacion *		ptr;
		int 				orden;

		item(uptr_representacion&& r, int orden=0)
			:rep(std::move(r)), ptr(rep.get()), orden(orden)
		{

		}

		item(DLibV::Representacion * p, int orden=0)
			:rep(nullptr), ptr(p), orden(orden)
		{}

		bool operator<(const item& o) const
		{
			return orden < o.orden;
		}

		void volcar(DLibV::Pantalla& p)
		{
			ptr->volcar(p);
		}

		void volcar(DLibV::Pantalla& p, const DLibV::Camara& cam)
		{
			if(!ptr->es_estatica()) ptr->volcar(p, cam);
			else ptr->volcar(p);
		}
	};


	uptr_representacion	crear_caja(const Dnot_token&);
	uptr_representacion	crear_bitmap(const Dnot_token&);
	uptr_representacion	crear_patron(const Dnot_token&);
	uptr_representacion	crear_texto(const Dnot_token&);
	uptr_representacion	crear_ttf(const Dnot_token&);
	void			procesar_tipo_pantalla(const Dnot_token&);
	void			procesar_tipo_constante(const Dnot_token&);

	SDL_Rect		caja_desde_lista(const Dnot_token&);
	color			color_desde_lista(const Dnot_token&);
	SDL_Color		rgba_desde_lista(const Dnot_token&);
	posicion		posicion_desde_lista(const Dnot_token&);

	std::vector<item>				representaciones;
	std::map<std::string, DLibV::Representacion*>	mapa_ids;
	std::map<std::string, DLibV::Representacion*>	mapa_externas;
	std::map<std::string, DLibV::Textura*>		mapa_texturas;
	std::map<std::string, DLibV::Superficie*>	mapa_superficies;
	std::map<std::string, const DLibV::Fuente_TTF*>	mapa_fuentes;
	std::map<std::string, int>			constantes_int;
	std::map<std::string, float>			constantes_float;

	bool 						con_pantalla;
	SDL_Color					color_pantalla;
};
}

/*
* El fichero de configuración debe tener tantos nodos base como escenas 
* configura.
* En nuestro caso, el nodo base de la escena se llama "escena_prueba". El nodo 
* base debe ser una lista de objetos, cada uno identificado por un "tipo". Todos
* los objetos pueden tener la clave "orden" y "alpha" para configurar el orden
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
* - patron:
*	pos[x, y, w, h] : posición de la caja
*	rec[x, y, w, h] : recorte de la textura
*	textura["cadena"] : cadena de mapeo de la textura
*	pincel:[x, y, w, h] : rectángulo de pincel que se usará para dibujar
* - texto:
*	pos[x, y] : posición del texto
*	superficie["cadena"] : cadena de mapeo de la superficie de texto
*	texto["texto"] : Texto que se dibujará
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
		orden:1
	},
	{
		tipo:"caja",
		pos:[80, 16, 40, 40], rgb:[0, 0, 255],
		orden:2
	},
	{
		tipo:"bitmap",
		pos:[200, 200, 14, 30], rec:[0, 0, 14, 30],
		textura:"sprites", alpha:192
	},
	{
		tipo:"patron",
		pos:[0, 128, 256, 64], 
		rec:[0, 0, 64, 32],
		pincel:[0, 0, 64, 32],
		textura:"tiles"
	},
	{
		tipo:"texto", 
		pos:[20, 150],
		superficie:"fuente",
		texto:"Hola... Salgo de un fichero."
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
