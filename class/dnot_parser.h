#ifndef DNOT_PARSER_H
#define DNOT_PARSER_H

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

namespace Herramientas_proyecto
{

class Dnot_parser
{
	private:

	enum class tipos {objeto, lista};

	public:

						Dnot_parser(std::istream&, tipos t=tipos::objeto);
	void 					operator()();

	//TODO: Un problema con el parser es que los ficheros no tienen un nodo
	//"raiz". Asumiremos siempre que la base es un objeto mientras no pensemos
	//en una solución.

	const Dnot_token&			acc_token() const 
	{
		if(!finalizado) throw std::runtime_error("El parser no se ha ejecutado");
		return token;
	}

	Dnot_token&				acc_token() 
	{
		if(!finalizado) throw std::runtime_error("El parser no se ha ejecutado");
		return token;
	}

//	std::map<std::string, Dnot_token>& 	acc_tokens() {return tokens;}
	
	private:
 
	
	char 					leer_string();
	char 					leer_stream();
	void 					procesar_string(char cb);
	void 					procesar_stream(char cb);
	void 					comillas();
	void 					coma();
	void 					abre_llave();
	void 					cierra_llave();
	void 					abre_corchete();
	void 					cierra_corchete();
	void 					asignar_valor_objeto();
	Dnot_token 				generar_token_valor(const std::string& v);
	void 					asignar_valor_lista();
	void 					asignar_subparser_objeto(const std::map<std::string, Dnot_token>& aux);
	void 					asignar_subparser_lista(const std::vector<Dnot_token>& aux);
	void 					error(const std::string& msj);
	std::string 				traducir_estado();

	enum class estados {leyendo, salir, fin_subparser};

	estados			estado;
	bool			leer_comillas, finalizado;
	tipos			tipo;
	std::istream&		stream; //El stream es una referencia para poder pasarlo a los parsers recursivos.
	std::string 		buffer;

	//TODO: std::map no conserva el orden de inserción: Tal vez necesitemos
	//un vector de pares std::string - token, para poder hacer esto.

	Dnot_token				token; //El token base.


	//std::map<std::string, Dnot_token>	tokens;
	//std::vector<Dnot_token>	lista;
};

Dnot_token	parsear_dnot(const std::string&);
}

/*
Dnot puede leer algo como...

inicio:1,
sencillo:{
	ps1:"una cadena con espacios y : y cosas así, con comas y {abre} y [cierra]",
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
	lista:[1,2,3, {uno:1, dos:2}, 4, 5, ["uno", "dos", "tres"]],
	obj:{a:"b", c:"d"}
}

Ignorará todo whitespace que no esté en una cadena y tolerará valores enteros,
de cadena, float a bool.

Un ejemplo de uso está en "base_proyecto", en github.
*/
#endif
