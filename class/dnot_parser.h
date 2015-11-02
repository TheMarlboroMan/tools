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
#include <fstream>
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

						Dnot_parser(std::ifstream& fichero, tipos t=tipos::objeto);
	void 					operator()();

	//TODO: Un problema con el parser es queS los ficheros no tienen un nodo
	//"raiz". Asumiremos siempre que la base es un objeto mientras no pensemos
	//en una solución.

	std::map<std::string, Dnot_token>& 	acc_tokens() {return tokens;}
	
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
	bool			leer_comillas;
	tipos			tipo;
	std::ifstream&		fichero;
	std::string 		buffer;

	//TODO: std::map no conserva el orden de inserción: Tal vez necesitemos
	//un vector de pares std::string - token, para poder hacer esto.

	std::map<std::string, Dnot_token>	tokens;
	std::vector<Dnot_token>	lista;
};

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
		cosa_dos:2.34
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

Este programilla leería el fichero.

void dime_token(const Dnot_token& t, int tabs=0)
{
	std::string ver_tabs;
	for(int i=0; i<tabs; ++i) ver_tabs+="\t";

	std::cout<<ver_tabs;

	if(t.es_valor())
	{
		if(t.es_valor_string()) std::cout<<"\""<<t.acc_string()<<"\""<<std::endl;
		else if(t.es_valor_int()) std::cout<<t.acc_int()<<std::endl;
		else if(t.es_valor_float()) std::cout<<t.acc_float()<<"f"<<std::endl;
		else if(t.es_valor_bool()) std::cout<<(t.acc_bool() ? "true" : "false")<<std::endl;	
	}
	else if(t.es_objeto())
	{
		std::cout<<ver_tabs<<"{"<<std::endl;
		const auto& tokens=t.acc_tokens();
		for(const auto& tok : tokens) 
		{
			std::cout<<tok.first<<":";
			dime_token(tok.second, tabs+1);
		}
		std::cout<<ver_tabs<<"}"<<std::endl;
	}
	else if(t.es_lista())
	{
		std::cout<<ver_tabs<<"["<<std::endl;
		const auto& lista=t.acc_lista();
		for(const auto& tok : lista) dime_token(tok, tabs+1);	
		std::cout<<ver_tabs<<"]"<<std::endl;
	}
	else std::cout<<"TIPO DESCONOCIDO"<<std::endl;
}

int main(int argc, char ** argv)
{
	try
	{
		std::ifstream fichero("ejemplo.dat");
		if(!fichero.is_open()) throw std::runtime_error("Imposible abrir fichero");

		Dnot_parser p(fichero);
		p();

		const auto& tokens=p.acc_tokens();	
		for(const auto& t : tokens)
		{
			std::cout<<t.first<<":";
			dime_token(t.second);
		}

		return 0;
	}
	catch(std::exception& e)
	{
		std::cout<<"ERROR : "<<e.what();
		return 1;
	}
}
*/

#endif
