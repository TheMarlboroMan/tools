#ifndef TOOLS_DNOT_PARSER_H
#define TOOLS_DNOT_PARSER_H

/**
* Parser de algo que se parece a JSON pero no termina de serlo porque no he
* contemplado todos los casos... Y lo llamaremos... Dnot :P. Algunas diferencias
* son que, de momento, no preserva el orden de inserción de elementos en un
* mapa asociativo y que puede tener X elementos de raiz.
* Al final de este fichero hay ejemplos comentados.
*/

#include <cctype>
#include <istream>

#include <fstream>
#include <iostream>

#include <stdexcept>
#include <algorithm>
#include "dnot_token.h"

namespace tools
{

class dnot_parser
{
	private:

	enum class types {tundefined, tmap, tvector};

	public:

						dnot_parser(std::istream&);
	void 					operator()();

	//TODO: Un problema con el parser es que los ficheros no tienen un nodo
	//"raiz". Asumiremos siempre que la base es un tobject mientras no pensemos
	//en una solución.

	const dnot_token&			get_token() const 
	{
		if(!done) throw std::runtime_error("dnot parser was not executed");
		return token;
	}

	dnot_token&				get_token() 
	{
		if(!done) throw std::runtime_error("dnot parser was not executed");
		return token;
	}

//	std::map<std::string, dnot_token>& 	get_tokens() {return tokens;}
	
	private:
 
	
	char 					read_string();
	char 					read_stream();
	void 					process_string(char cb);
	void 					process_stream(char cb);
	void 					quotes();
	void 					comma();
	void 					open_brace();
	void 					close_brace();
	void 					open_bracket();
	void 					close_bracket();
	void 					assign_tobject();
	dnot_token 				generate_token(const std::string& v);
	void 					assign_tarray();
	void 					assign_tobject_subparser(const std::map<std::string, dnot_token>& aux);
	void 					assign_tarray_subparser(const std::vector<dnot_token>& aux);
	void 					error(const std::string& msj);
	std::string 				get_state();

	enum class tstates {reading, exiting, exiting_subparser};

	tstates			state;
	bool			read_quotes, 
				done,
				root;
	types			type;
	std::istream&		stream; //El stream es una referencia para poder pasarlo a los parsers recursivos.
	std::string 		buffer;

	//TODO: std::map no conserva el orden de inserción: Tal vez necesitemos
	//un vector de pares std::string - token, para poder hacer esto.

	dnot_token				token; //El token base.


	//std::map<std::string, dnot_token>	tokens;
	//std::vector<dnot_token>	tarray;
};

dnot_token	dnot_parse(const std::string&);
}

/*
Dnot puede leer algo como...

inicio:1,
sencillo:{
	ps1:"una cadena con espacios y : y cosas así, con commas y {abre} y [cierra]",
	ps2:12
34
},

complejo:{
	parametro_complejo_uno:true,
	parametro_complejo_dos:{
		cosa_uno: "Hola dos puntos : hola",
		cosa_dos:2.34f,
		cosa_tres:2.340343243
	},
	parametro_complejo_tres:-3,
	parametro_complejo_cuatro:{
		p_a:{
			uno:1,
			dos:2},
		p_b:"beee"
	},
	tarray:[1,2,3, {uno:1, dos:2}, 4, 5, ["uno", "dos", "tres"]],
	obj:{a:"b", c:"d"}
}

Ignorará todo whitespace que no esté en una cadena y tolerará valores enteros,
de cadena, float a bool.

Un ejemplo de uso está en "base_proyecto", en github.
*/
#endif
