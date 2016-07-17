#include "localization_base.h"
#include <cstdlib>
#include "../templates/compatibility_patches.h"

using namespace tools;

localization_base::localization_base(unsigned short int p_language)
{
	this->language=p_language;
}

localization_base::~localization_base()
{
	this->clear();
}

void localization_base::clear()
{
	this->data.clear();
}

void localization_base::set_language(unsigned short int p_language)
{
	this->language=p_language;
	this->init();
}

void localization_base::insert(unsigned int pindex, t_string const& pstring)
{
	this->data.insert(std::pair<unsigned int, t_string>(pindex, pstring));
}

localization_base::t_string localization_base::get_filename(t_string const& p_original)
{
	t_string nombre_archivo(p_original);

	nombre_archivo.append(".");
	nombre_archivo.append(compat::to_string(this->language));
	nombre_archivo.append(".dat");

	return nombre_archivo;
}

void localization_base::init()
{
	t_filename lista=this->get_file_list();
	t_filename::iterator ini=lista.begin(), fin=lista.end();

	//Limpiar mapa e ir asignando.
	this->clear();

	for(;ini<fin; ini++)
	{
		this->process_file(*ini);
	}

}

void localization_base::process_file(t_string const& nombre_archivo)
{
	const std::string ruta=get_filename(nombre_archivo);

	t_stream_in archivo(ruta.c_str(), std::ios::in | std::ios::binary);
	if(!archivo)
	{
		throw std::runtime_error("Unable to load localization file "+ruta);
	}
	else
	{
		size_t indice=0; //, pos;
		size_t indice_aux=0;
		t_string_stream cadena, cadena_def;
		bool leyendo=false;

		//Leemos de línea en línea, 1024 como mucho. Por cada línea leida procesamos.
		while(true)
		{
			std::getline(archivo, cadena);
			if(archivo.eof()) 
			{
				break;
			}


			//La cadena esta vacia?	Si no estamos leyendo saltar al siguiente con "continue".
			if(!cadena.size() && !leyendo) 
			{
				continue;
			}
			else if(cadena[0]=='#') 
			{
				continue;	//Es un comentario????
			}

			//Delimitador de inicio encontrado?
		
			if(begin_delimiter(cadena, indice_aux))
			{
				leyendo=true; //Marcar leyendo como true.
				indice=indice_aux; //Obtener índice.
				cadena=cadena.substr(3+int_digits(indice)); //Cortar delimitador inicio. + 3 por el < y el $>
			}

			//Delimitador de fin encontrado?
			if(this->end_delimiter(cadena))
			{
				leyendo=false; //Marcar leyendo como false.
				cadena=cadena.substr(0, cadena.size()-3); //Cortar resto cadena. -3 es por <#>
				cadena_def.append(cadena);

				//Insertar en mapa.

				this->insert(indice, cadena_def);
				cadena_def.clear();
				cadena.clear();
			}		

			//Estamos leyendo?
			if(leyendo)
			{
				cadena.append("\n"); //Insertar nueva línea.
				cadena_def.append(cadena); //Insertar en cadena actual.	
				cadena.clear();	//Limpiar buffer.
			}
		}	
	
		archivo.close();
	}

//	delete [] buffer;
}

/*
Busca <n#> al principio de la cadena donde n# es un número positivo. Si lo
encuentra devuelve n#. Si no devolverá -1.
*/
bool localization_base::begin_delimiter(std::string const& pstring, size_t &indice)
{
	size_t pos=pstring.find("$>", 1);
	std::string cad_indice("");

	if(pos!=std::string::npos)
	{
		cad_indice.assign(pstring.substr(1, pos-1));
		indice=std::atoi(cad_indice.c_str());
		return true;
	}
	else 
	{
		return false;
	}
}

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

localization_base::t_string const& localization_base::get(unsigned int pindex) const
{
	if(!this->data.size())
	{
		return this->string_not_found();
	}
	else
	{
		const auto it=this->data.find(pindex);

		if(it==this->data.end())
		{
			return this->string_not_found();
		}
		else
		{
			return it->second;
		}
	}
}
