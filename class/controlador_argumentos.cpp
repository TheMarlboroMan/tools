#include "controlador_argumentos.h"
#include <algorithm>
#include <iostream>
#include <sstream>

using namespace Herramientas_proyecto;

Controlador_argumentos::Controlador_argumentos()
{

}

Controlador_argumentos::Controlador_argumentos(int argc, char ** argv)
{
	this->inicializar(argc, argv);
}

void Controlador_argumentos::inicializar(int argc, char ** argv)
{
	int i=0;
	for(; i<argc; i++)
	{
		this->argumentos.push_back(t_argumento(argv[i]));
	}
}

Controlador_argumentos::~Controlador_argumentos()
{
	this->argumentos.clear();
}

const Controlador_argumentos::t_argumento Controlador_argumentos::acc_argumento(unsigned int p_arg) const
{
	try
	{
		return this->argumentos[p_arg];
	}
	catch (...)
	{
		throw Controlador_argumentos_exception("se intentó acceder a un argumento inexistente");
//		std::cout<<"Controlador_argumentos::se intentó acceder a un argumento no existente"<<std::endl;
//		return t_argumento("ERROR");
	}
}

const Controlador_argumentos::t_lista_argumentos& Controlador_argumentos::acc_argumentos() const 
{
	return this->argumentos;
}

/*
Busca un argumento. Devuelve -1 si no encuentra nada y el índice del argumento si la ha encontrado.
*/

int Controlador_argumentos::buscar(const char * p_char) const {return this->buscar(t_argumento(p_char));}
int Controlador_argumentos::buscar(t_argumento const& p_buscar) const
{
	//Manual del todo..
	int resultado=-1;
	int i=0;

	t_lista_argumentos::const_iterator 	ini=this->argumentos.begin(),
						fin=this->argumentos.end();

	for(;ini<fin; ini++, i++)
	{
		if( (*ini).compare(p_buscar)==0)
		{
			resultado=i;
			break;
		}
	}

	return resultado;
}

/*Devuelve el valor del argumento "argumento" si se localiza en la cadena. Se entiende 
"argumento" cuando se especifica de la siguente forma: argumento=valor y argumento queda
a la izquierda del separador (en este caso =)
Si no se localiza el argumento se lanza una excepción propia para indicarlo.
*/

std::string Controlador_argumentos::valor_argumento(const char *c, const char separador) const
{
	std::string cad=c;
	return valor_argumento(cad, separador);
}

std::string Controlador_argumentos::valor_argumento(const std::string& argumento, const char separador) const
{
	std::stringstream ss;
	ss<<argumento<<separador;
	const std::string buscar=ss.str();

	auto it=std::find_if(std::begin(argumentos), std::end(argumentos), [&buscar](const std::string& arg)
		{return arg.find(buscar)!=std::string::npos;});

	if(it==argumentos.end())
	{
		const std::string cad_error="No se localiza el argumento "+argumento;
		throw Controlador_argumentos_exception(cad_error);
	}
	else
	{
		auto ex=explotar(*it, separador);
		if(ex.size()!=2)
		{
			const std::string cad_error="El argumento "+argumento+" contiene más de un separador";
			throw Controlador_argumentos_exception(cad_error);
		}
		else
		{
			return ex[1];
		}
	}
}
