#ifndef TOOLS_LOCALIZATION_BASE_H
#define TOOLS_LOCALIZATION_BASE_H

#include <string>
#include <map>
#include <iostream>
#include <fstream>
#include <cstring> //Para memset
#include <utility> //para "pair"
#include <vector>
#include <stdexcept>

#include "../source/string_utils.h"

namespace tools
{

class localization_base
{
	public:

	//typedef std::map<unsigned int, std::wstring> t_map;	//Tipo del mapa.
	//typedef std::wstring t_string;				//Tipo de cadena que devuelve.
	typedef std::map<unsigned int, std::string> t_map;	//Tipo del mapa.
	typedef std::string t_string;				//Tipo de cadena que devuelve.
	typedef std::string t_string_stream;				//Tipo de cadena que devuelve.
	typedef char t_char_buffer;				//Tipo del buffer de lectura del archivo.
	typedef std::ifstream t_stream_in;			//Tipo del archivo que abre.
	typedef std::vector<t_string> t_filename;	//Tipo de dato que devuelve componer_nombre_archivo;

	enum languages{lspanish=0, lenglish=1};

				localization_base(unsigned short int);
	virtual 		~localization_base();

	void 			set_language(unsigned short int);
	t_string const& 	get(unsigned int) const;

	virtual void 		init();
	void 			reload() {init();}

	//A implementar...

	protected:

	t_map 			data;
	unsigned short int 	language;

	bool 			begin_delimiter(t_string const&, size_t&);
	bool 			end_delimiter(t_string const&);
	void 			clear();
	void 			insert(unsigned int, t_string const&);
	t_string 		get_filename(t_string const&);
	void 			process_file(t_string const&);

	virtual t_filename 	get_file_list()=0;
	virtual t_string const& string_not_loaded()const=0;
	virtual t_string const& string_not_found()const=0;
};

}
#endif
