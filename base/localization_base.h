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

//!Base localization class.

//!Implementing the three protected virtual methods will prepare the class.
//!Files are expected to be plain text files. Commented lines begin with #.
//!A localised string is in this format: <n#>String<#> where n is unique
//!positive number identifying it.

class localization_base
{
	public:

	typedef std::map<unsigned int, std::string> t_map;	//!< Internal map type.
	typedef std::string t_string;				//!< Returned type from "get".
	typedef std::string t_string_stream;			//!< Internal stream type.
	typedef char t_char_buffer;				//!< Internal buffer type.
	typedef std::ifstream t_stream_in;			//!< Input file type.
	typedef std::vector<t_string> t_filename;		//!< Returned value for "get_file_list".

				localization_base(unsigned short int);
	virtual 		~localization_base();

	void 			set_language(unsigned short int);
	t_string const& 	get(unsigned int) const;
	void			init();

	//!When implemented must return a set of files to read. The files must have a complete absolute or relative path but must not include any extension.
	//!Filenames are expressed as "filename.integerlanguage.dat", thus .integerlanguage.dat must be ommited.
	virtual t_filename 	get_file_list()=0;
	
	//!When implemented, must return a string indicating that the localization is not initialised.
	virtual t_string const& string_not_loaded()const=0;

	//!When implemented, must return a string indicating that the index does not exist.
	virtual t_string const& string_not_found()const=0;

	private:

	bool 			begin_delimiter(t_string const&, size_t&);
	bool 			end_delimiter(t_string const&);
	void 			clear();
	void 			insert(unsigned int, t_string const&);
	t_string 		compose_filename(t_string const&);
	void 			process_file(t_string const&);

	t_map 			data;
	unsigned short int 	language;


};

}
#endif
