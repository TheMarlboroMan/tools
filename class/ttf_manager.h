#ifndef TOOLS_TTF_MANAGER_H
#define TOOLS_TTF_MANAGER_H

#include <map>
#include <string>

#include <video/ttf_font/ttf_font.h>

/*
Un gestor de data ttf. Almacena una clave de texto o y un entero con una fuente
ttf y un sizeaño. Se trabaja en dos pasos:

- 1: registrar la fuente con registrar_fuente("alias", sizeaño, "ruta_ttf");
- 2: obtener la referencia a la fuente con obtener_fuente("alias"; sizeaño);

En caso de no existir la fuente se lanzará un std::runtime_error.
*/

namespace tools
{

class ttf_manager
{
	public:

	const ldv::ttf_font&				get(const std::string&, int) const;
	void						insert(const std::string&, int, const std::string&);

	private:

	struct font_info
	{
		std::string name;
		int size;
		bool operator<(const font_info& o) const
		{
			if(name < o.name) return false;
			else if(name > o.name) return true;
			else return size < o.size;
		}
	};

	std::map<font_info, ldv::ttf_font>	data;
};

}

#endif
