#ifndef TOOLS_PAIR_FILE_PARSER_H
#define TOOLS_PAIR_FILE_PARSER_H

#include <map>
#include <exception>
#include "../source/string_utils.h"

/**
* Clase wrapper para leer un fichero de configuración y volcar
* sus contenidos en un mapa de std::string. Cuando se cargue la
* información del fichero original se descartarán todos los 
* comments y líneas en blanco (no aparecerán al save). 
* De la misma forma, al usarse un mapa como contenedor, no
* se saveá el orden original de las entradas.
*/

namespace tools
{

class pair_file_parser
{
	public:
						pair_file_parser(const std::string&, char, char);

	void 					load();
	void					save();
	void					sync(const pair_file_parser& f);
	const std::string&			operator[](const std::string key) const {return data.at(key);}
	std::string&				operator[](const std::string key) {return data[key];}

	private:

	std::string				filename;
	char					delimiter,
						comment;

	std::map<std::string, std::string>	data;
};

}

#endif
