#ifndef CONTROLADOR_ARGUMENTOS_H
#define CONTROLADOR_ARGUMENTOS_H

#include <vector>
#include <string>
#include <stdexcept>
#include "../source/string_utilidades.h"

/*
Una clase simple que almacenará los argumentos que se pasen al programa.
*/

namespace Herramientas_proyecto
{

class Controlador_argumentos_exception:public std::runtime_error
{
	public:

	Controlador_argumentos_exception(const std::string& c):std::runtime_error(c) {}
};

class Controlador_argumentos
{
	private:
	typedef std::vector<std::string> t_lista_argumentos;
	typedef std::string t_argumento;
	
	t_lista_argumentos argumentos;	

	public:

	Controlador_argumentos();
	Controlador_argumentos(int argc, char ** argv);
	~Controlador_argumentos();

	int 			buscar(const char * p_char) const;
	int 			buscar(t_argumento const& p_buscar) const;
	void 			inicializar(int argc, char ** argv);
	size_t 			obtener_cantidad() const {return argumentos.size();}
	size_t 			size() const {return argumentos.size();}
	const 			t_argumento acc_argumento(unsigned int p_arg) const;
	const 			t_lista_argumentos& acc_argumentos() const;
	bool			argumento_existe(const char * p_char) const {return buscar(p_char)!=-1;}
	//TODO: Mejor, un método al que pasemos un callable.. Y que itere.

	std::string valor_argumento(const char *c, const char separador='=') const;
	std::string valor_argumento(const std::string& c, const char separador='=') const;
};

}

#endif
