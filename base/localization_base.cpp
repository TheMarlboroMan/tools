#include "localization_base.h"
#include <cstdlib>
#include "../templates/compatibility_patches.h"

using namespace tools;

//!Default constructor. 

//!Initialises the object with the language identified by the parameter, which
//!is application dependent.

localization_base::localization_base(unsigned short int p_language)
	:language(p_language)
{

}

//!Class destructor.

localization_base::~localization_base()
{
	clear();
}

//!Needs to be called after constructor. A good idea is to put it in the constructor of the derived class.

void localization_base::init()
{
	clear();
	for(const auto& f: get_file_list()) process_file(f);
}

//!Removes all localization strings.

void localization_base::clear()
{
	data.clear();
}

//!Sets the language.

//!The language is identified by an integer in an application dependent manner.
//!As a side effect, the set of localized strings for the language is loaded
//!and the previous are removed.

void localization_base::set_language(unsigned short int p_language)
{
	language=p_language;
	init();
}

//!Inserts a localised string. Internal method.

void localization_base::insert(unsigned int pindex, t_string const& pstring)
{
	data.insert(std::pair<unsigned int, t_string>(pindex, pstring));
}

//!Composes the filename. Internal method.

//!Filenames are expressed as "filename.#integerlanguage#.dat".

localization_base::t_string localization_base::compose_filename(t_string const& p_original)
{
	t_string nombre_archivo(p_original);

	nombre_archivo.append(".");
	nombre_archivo.append(compat::to_string(language));
	nombre_archivo.append(".dat");

	return nombre_archivo;
}

//!Loads strings from a file. Internal method. 

//!Will throw if the file cannot be found.

void localization_base::process_file(t_string const& nombre_archivo)
{
	const std::string ruta=compose_filename(nombre_archivo);

	t_stream_in archivo(ruta.c_str(), std::ios::in | std::ios::binary);
	if(!archivo)
	{
		throw std::runtime_error("Unable to load localization file "+ruta);
	}
	else
	{
		size_t index=0; //, pos;
		size_t index_aux=0;
		t_string_stream cadena, cadena_def;
		bool reading=false;

		//Read and process the line.
		while(true)
		{
			std::getline(archivo, cadena);
			if(archivo.eof()) 
			{
				break;
			}

			//Empty string?. If not reading, just jump to the next.
			if(!cadena.size() && !reading) 
			{
				continue;
			}
			//Did we find a comment?
			else if(cadena[0]=='#') 
			{
				continue;
			}

			//Did we find a "begin delimiter"
			if(begin_delimiter(cadena, index_aux))
			{
				reading=true;
				index=index_aux;
				cadena=cadena.substr(3+int_digits(index)); //Cut delimiter... +3 is because of <$>
			}

			//Did we find an "end delimiter"?
			if(end_delimiter(cadena))
			{
				reading=false;
				cadena=cadena.substr(0, cadena.size()-3); //-3 es por <#>
				cadena_def.append(cadena);

				insert(index, cadena_def);
				cadena_def.clear();
				cadena.clear();
			}		

			//Are we reading?
			if(reading)
			{
				cadena.append("\n"); //Add new line.
				cadena_def.append(cadena); //Append to current.
				cadena.clear();	//Clear.
			}
		}	
	
		archivo.close();
	}
}

//!Locates the begin delimiter mark. Internal method.

bool localization_base::begin_delimiter(std::string const& pstring, size_t &index)
{
	size_t pos=pstring.find("$>", 1);
	std::string cad_index("");

	if(pos!=std::string::npos)
	{
		cad_index.assign(pstring.substr(1, pos-1));
		index=std::atoi(cad_index.c_str());
		return true;
	}
	else 
	{
		return false;
	}
}

//!Locates the end delimiter mark. Internal function.

bool localization_base::end_delimiter(std::string const& pstring)
{
	bool resultado=false;
	size_t pos;
	pos=pstring.find("<#>");

	if(pos!=std::string::npos)
	{
		resultado=true;
	}

	return resultado;
}

//!Returns the localised string with the requested index.

//!If the value is not found it will return a special, application dependent
//!string.

localization_base::t_string const& localization_base::get(unsigned int pindex) const
{
	if(!data.size())
	{
		return string_not_found();
	}
	else
	{
		const auto it=data.find(pindex);
		return it==data.end() ? string_not_found() : it->second;
	}
}
