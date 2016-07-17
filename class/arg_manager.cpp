#include "arg_manager.h"
#include <algorithm>
#include <iostream>
#include <sstream>

using namespace tools;

arg_manager::arg_manager(int argc, char ** argv)
{
	init(argc, argv);
}

void arg_manager::init(int argc, char ** argv)
{
	int i=0;
	for(; i<argc; i++)
	{
		data.push_back(t_arg(argv[i]));
	}
}

const arg_manager::t_arg arg_manager::get_argument(unsigned int p_arg) const
{
	try
	{
		return data[p_arg];
	}
	catch (...)
	{
		throw std::runtime_error("Invalid argument index");
	}
}

/*
Busca un argumento. Devuelve -1 si no encuentra nada y el índice del argumento si la ha encontrado.
*/

int arg_manager::find_index(const char * p_char) const {return find_index(t_arg(p_char));}
int arg_manager::find_index(t_arg const& p_find_index) const
{
	//Manual del todo..
	int result=-1;
	int i=0;

	t_arg_list::const_iterator 	ini=data.begin(),
						fin=data.end();

	for(;ini<fin; ini++, i++)
	{
		if( (*ini).compare(p_find_index)==0)
		{
			result=i;
			break;
		}
	}

	return result;
}

/*Devuelve el valor del argumento "argumento" si se localiza en la cadena. Se entiende 
"argumento" cuando se especifica de la siguente forma: argumento=valor y argumento queda
a la izquierda del delimiter (en este caso =)
Si no se localiza el argumento se lanza una excepción propia para indicarlo.
*/

std::string arg_manager::get_value(const char *c, const char delimiter) const
{
	std::string cad=c;
	return get_value(cad, delimiter);
}

std::string arg_manager::get_value(const std::string& argumento, const char delimiter) const
{
	std::stringstream ss;
	ss<<argumento<<delimiter;
	const std::string find_index=ss.str();

	auto it=std::find_if(std::begin(data), std::end(data), [&find_index](const std::string& arg)
		{return arg.find(find_index)!=std::string::npos;});

	if(it==data.end())
	{
		throw std::runtime_error("Unable to locate argument "+argumento);
	}
	else
	{
		auto ex=explode(*it, delimiter);
		if(ex.size()!=2)
		{
			throw std::runtime_error("Invalid delimiter for argument "+argumento);
		}
		else
		{
			return ex[1];
		}
	}
}
