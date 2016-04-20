#ifndef GESTOR_FUENTES_TTF_H
#define GESTOR_FUENTES_TTF_H

#include <map>
#include <string>

#include <video/fuente_ttf/fuente_ttf.h>

/*
Un gestor de fuentes ttf. Almacena una clave de texto o y un entero con una fuente
ttf y un tamaño. Se trabaja en dos pasos:

- 1: registrar la fuente con registrar_fuente("alias", tamaño, "ruta_ttf");
- 2: obtener la referencia a la fuente con obtener_fuente("alias"; tamaño);

En caso de no existir la fuente se lanzará un std::runtime_error.
*/

namespace Herramientas_proyecto
{

class Gestor_fuentes_TTF
{
	public:

	const DLibV::Fuente_TTF&			obtener_fuente(const std::string&, int) const;
	void						registrar_fuente(const std::string&, int, const std::string&);

	private:

	struct info_fuente
	{
		std::string nombre;
		int tam;
		bool operator<(const info_fuente& o) const
		{
			if(nombre < o.nombre) return false;
			else if(nombre > o.nombre) return true;
			else return tam < o.tam;
		}
	};

	std::map<info_fuente, DLibV::Fuente_TTF>	fuentes;
};

}

#endif
