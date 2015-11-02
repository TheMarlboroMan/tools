#ifndef DNOT_TOKEN_H
#define DNOT_TOKEN_H

#include <vector>
#include <map>
#include <string>
#include <stdexcept>

/**
* Tokens para el parser de Dnot.
*/

namespace Herramientas_proyecto
{

class Dnot_Parser;

class Dnot_token
{
	public:

	bool 					es_valor() const;
	bool 					es_valor_string() const {return tipo==tipos::valor_string;}
	bool 					es_valor_int() const {return tipo==tipos::valor_int;}
	bool 					es_valor_float() const {return tipo==tipos::valor_float;}
	bool 					es_valor_bool() const {return tipo==tipos::valor_bool;}
	bool 					es_objeto() const {return tipo==tipos::compuesto;}
	bool 					es_lista() const {return tipo==tipos::lista;}

	void 					asignar(const std::string c);
	void		 			asignar(int c);
	void 					asignar(float c);
	void 					asignar(bool c);
	void 					asignar(const std::map<std::string, Dnot_token>& t);
	void 					asignar(const std::vector<Dnot_token>& t);

	const std::map<std::string, Dnot_token>& acc_tokens() const {return tokens;}
	const std::vector<Dnot_token>& 		acc_lista() const {return lista;}
	const std::string& 			acc_string() const {return valor_string;}
	int 					acc_int() const {return valor_int;}
	float 					acc_float() const {return valor_float;}
	bool 					acc_bool() const {return valor_bool;}

						Dnot_token();
						explicit Dnot_token(const std::string& v);
						explicit Dnot_token(int v);
						explicit Dnot_token(float v);
						explicit Dnot_token(bool v);
	private:

	enum class tipos {
		compuesto, 	//El token es un objeto...
		valor_string,	//El token tiene un valor string
		valor_int,	//El token tiene un valor int
		valor_float,	//El token tiene un valor float
		valor_bool,	//El token tiene un valor bool
		lista};		//El token es una lista de otros tokens anónimos.

	tipos					tipo;

	std::map<std::string, Dnot_token>	tokens;
	std::vector<Dnot_token>			lista;
	std::string				valor_string;
	int					valor_int;
	float					valor_float;
	bool					valor_bool;

	friend class Dnot_Parser;
};
}

#endif
