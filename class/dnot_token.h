#ifndef DNOT_TOKEN_H
#define DNOT_TOKEN_H

#include <iostream>
#include <vector>
#include <map>
#include <string>
#include <stdexcept>

/**
* Tokens para el parser de Dnot.
*
* El token puede ser (simultáneamente, el código no lo impide) una cadena,
* un entero, booleano, float, array o mapa. Lógicamente, sólo vamos a usar uno 
* de ellos a la vez pero no se lanza ninguna excepción (de momento) si 
* intentamos coger un float cuando es cadena (devolvería 0.0f).
*
*
*/

namespace Herramientas_proyecto
{

class Dnot_Parser;

struct Dnot_token_opciones_serializador
{
	bool 		tabular_profundidad;
	std::string	tabulador;
	Dnot_token_opciones_serializador()
		:tabular_profundidad(false),
		tabulador("\t")
	{}
};

class Dnot_token
{
	public:

	std::string				serializar(const Dnot_token_opciones_serializador& = Dnot_token_opciones_serializador(), int=0) const;

	typedef					std::pair<std::string, Dnot_token> par_mapa;
	typedef 				std::map<std::string, Dnot_token> t_mapa;
	typedef					std::vector<Dnot_token> t_vector;

	bool 					es_valor() const;
	bool 					es_valor_string() const {return tipo==tipos::valor_string;}
	bool 					es_valor_int() const {return tipo==tipos::valor_int;}
	bool 					es_valor_float() const {return tipo==tipos::valor_float;}
	bool 					es_valor_double() const {return tipo==tipos::valor_double;}
	bool 					es_valor_bool() const {return tipo==tipos::valor_bool;}
	bool 					es_compuesto() const {return tipo==tipos::compuesto;}
	bool 					es_lista() const {return tipo==tipos::lista;}

	void 					asignar(const std::string c);
	void 					asignar(const char * c);
	void		 			asignar(int c);
	void 					asignar(float c);
	void 					asignar(double c);
	void 					asignar(bool c);
	void 					asignar(const t_mapa& t);
	void 					asignar(const t_vector& t);

	const t_mapa& 				acc_tokens() const;
	t_mapa&					acc_tokens();
	const t_vector& 			acc_lista() const;
	t_vector& 				acc_lista();
	const std::string& 			acc_string() const;
	int 					acc_int() const;
	float 					acc_float() const;
	double 					acc_double() const;
	bool 					acc_bool() const;

	bool 					existe_clave(const std::string&) const;

	const Dnot_token&			operator[](const std::string&) const;
	Dnot_token&				operator[](const std::string&);
	const Dnot_token&			operator[](const char *) const;
	Dnot_token&				operator[](const char *);
	const Dnot_token&			operator[](size_t) const;
	Dnot_token&				operator[](size_t);
	const Dnot_token&			operator[](int) const;
	Dnot_token&				operator[](int);

	//TODO...
/*
	bool 					operator==(const std::string&) const;
	bool 					operator==(const char *) const;
	bool 					operator==(int) const;
	bool 					operator==(float) const;
	bool 					operator==(bool) const;
	bool 					operator!=(const std::string&) const;
	bool 					operator!=(const char *) const;
	bool 					operator!=(int) const;
	bool 					operator!=(float) const;
	bool 					operator!=(bool) const;
*/

//	operator				t_mapa& () const {return tokens;}
//	operator				t_vector& () const {return lista;}

	operator 				std::string() const {return acc_string();}
	operator				int() const {return acc_int();}
	operator				bool() const {return acc_bool();}
	operator				float() const {return acc_float();}
	operator				double() const {return acc_double();}

						Dnot_token();
	explicit				Dnot_token(const std::string& v);
	explicit 				Dnot_token(const char *);
	explicit 				Dnot_token(const char);
	explicit 				Dnot_token(int v);
	explicit 				Dnot_token(float v);
	explicit 				Dnot_token(double v);
	explicit 				Dnot_token(bool v);
	explicit 				Dnot_token(const t_mapa& v);
	explicit 				Dnot_token(const t_vector& v);
	private:

	enum class tipos {
		compuesto, 	//El token es un objeto...
		valor_string,	//El token tiene un valor string
		valor_int,	//El token tiene un valor int
		valor_float,	//El token tiene un valor float
		valor_double,	//El token tiene un valor double
		valor_bool,	//El token tiene un valor bool
		lista};		//El token es una lista de otros tokens anónimos.

	std::string				traducir_tipo(tipos)const;
	std::string				valor_como_string()const;

	tipos					tipo;
	t_mapa					tokens;
	t_vector				lista;
	std::string				valor_string;
	int					valor_int;
	float					valor_float;
	double					valor_double;
	bool					valor_bool;

	friend class Dnot_parser;
	friend std::ostream& operator<<(std::ostream& os, const Herramientas_proyecto::Dnot_token& t);
};

std::ostream& operator<<(std::ostream& os, const Herramientas_proyecto::Dnot_token& t);

}

#endif
