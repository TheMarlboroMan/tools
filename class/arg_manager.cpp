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

int arg_manager::find_index(const t_arg& val) const
{
	int i=0;

	for(const auto& arg: data)
	{
		if(val==arg) return i;
		else ++i;
	}

	return -1;
}

int arg_manager::find_index_value(const t_arg& val) const
{
	int i=0;

	for(const auto& arg: data)
	{
		if(arg.substr(0, val.size())== val) return i;
		else ++i;
	}

	return -1;
}

/*Devuelve el valor del argumento "argumento" si se localiza en la cadena. Se entiende 
"argumento" cuando se especifica de la siguente forma: argumento=valor y argumento queda
a la izquierda del delimiter (en este caso =)
Si no se localiza el argumento se lanza una excepción propia para indicarlo.
*/

std::string arg_manager::get_value(const t_arg& argumento, const char delimiter) const
{
	std::stringstream ss;
	ss<<argumento<<delimiter;
	const std::string f_index=ss.str();

	auto it=std::find_if(std::begin(data), std::end(data), [&f_index](const std::string& arg)
		{return arg.find(f_index)!=std::string::npos;});

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
