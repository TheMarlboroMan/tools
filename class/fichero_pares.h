#ifndef FICHERO_PARES_H
#define FICHERO_PARES_H

#include <map>
#include <exception>
#include "../source/string_utilidades.h"

/**
* Clase wrapper para leer un fichero de configuración y volcar
* sus contenidos en un mapa de std::string. Cuando se cargue la
* información del fichero original se descartarán todos los 
* comentarios y líneas en blanco (no aparecerán al guardar). 
* De la misma forma, al usarse un mapa como contenedor, no
* se guardará el orden original de las entradas.
*/

namespace Herramientas_proyecto
{

class Fichero_pares_exception:public std::runtime_error
{
	public: 
	Fichero_pares_exception(const std::string& ruta)
		:std::runtime_error("Error al generar mapa de pares : no se ha podido abrir el archivo "+ruta)
	{}
};

class Fichero_pares
{
	public:
						Fichero_pares(const std::string&, char, char);

	void 					cargar();
	void					guardar();
	void					sincronizar(const Fichero_pares& f);
	const std::string&			operator[](const std::string clave) const {return configuracion.at(clave);}
	std::string&				operator[](const std::string clave) {return configuracion[clave];}

	private:

	std::string				nombre_fichero;
	char					separador;
	char					comentario;

	std::map<std::string, std::string>	configuracion;
};

}

#endif
