#ifndef TABLA_ANIMACIONES_H
#define TABLA_ANIMACIONES_H

#include <map>
#include <vector>
#include <stdexcept>
#include <sstream>
#include <iterator>

#include "tabla_sprites.h"

namespace Herramientas_proyecto
{

struct Linea_animacion
{
	public:
					Linea_animacion(float d, float m, const Frame_sprites& f)
						:duracion(d), momento_aparicion(m), frame(f) {}
					explicit operator bool() const {return duracion && frame;}
	float 				duracion=0.0f;				//Cuánto dura.
	float				momento_aparicion=0.0f;			//Cuál es el último momento de su duración dentro de toda la animación.
	Frame_sprites 			frame;
};

class Tabla_animaciones;

class Animacion
{
					//No necesita constructor: el float se inicializa a 0.0f...
	public:
					Animacion();
					explicit operator bool() const {return duracion_total > 0.0 || lineas.size() > 0;}
	const Linea_animacion&		obtener(size_t v) const {return lineas.at(v);}
	Linea_animacion			obtener(size_t v) {return lineas.at(v);}
	const Linea_animacion&		obtener_para_tiempo_animacion(float) const;
	const Linea_animacion&		obtener_para_tiempo_animacion(float, float) const;
	size_t				size() const {return lineas.size();}
	const std::string&		acc_nombre() const {return nombre;}
	float				acc_duracion_total() const {return duracion_total;}

	private:

	void				reajustar_tiempo_frames();

	std::string			nombre;
	std::vector<Linea_animacion>	lineas;
	float				duracion_total;

	friend class Tabla_animaciones;

};

class Tabla_animaciones
{
	public:
					Tabla_animaciones(const Tabla_sprites&);
					Tabla_animaciones(const Tabla_sprites&, const std::string&);
	void 				cargar(const std::string&);
	const Animacion& 		obtener(size_t v) const {return animaciones.at(v);}
	Animacion			obtener(size_t v) {return animaciones[v];}
	size_t				size() {return animaciones.size();}
	const Tabla_sprites&		acc_tabla_sprites() const {return tabla_sprites;}

	private:

	size_t				interpretar_como_cabecera(const std::string&);
	void				interpretar_como_linea(const std::string&, Animacion&);
	const Tabla_sprites&		tabla_sprites;
	std::map<size_t, Animacion>	animaciones;
};

}

#endif
