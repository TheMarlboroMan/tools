#ifndef TOOLS_DNOT_TOKEN_H
#define TOOLS_DNOT_TOKEN_H

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

namespace tools
{

class dnot_parser;

struct dnot_token_serialize_options
{
	bool 		tab_depth;
	std::string	tab_str;
			dnot_token_serialize_options()
		:tab_depth(false), tab_str("\t")
	{
	}
};

class dnot_token
{
	public:

	std::string				serialize(const dnot_token_serialize_options& = dnot_token_serialize_options(), int=0) const;

	typedef					std::pair<std::string, dnot_token> t_map_pair;
	typedef 				std::map<std::string, dnot_token> t_map;
	typedef					std::vector<dnot_token> t_vector;

	bool 					is_value() const;
	bool 					is_string_value() const {return type==types::tstring;}
	bool 					is_int_value() const {return type==types::tint;}
	bool 					is_float_value() const {return type==types::tfloat;}
	bool 					is_double_value() const {return type==types::tdouble;}
	bool 					is_bool_value() const {return type==types::tbool;}
	bool 					is_map() const {return type==types::tmap;}
	bool 					is_vector() const {return type==types::tvector;}

	void 					assign(const std::string c);
	void 					assign(const char * c);
	void		 			assign(int c);
	void 					assign(float c);
	void 					assign(double c);
	void 					assign(bool c);
	void 					assign(const t_map& t);
	void 					assign(const t_vector& t);

	const t_map& 				get_map() const;
	t_map&					get_map();
	const t_vector& 			get_vector() const;
	t_vector& 				get_vector();
	const std::string& 			get_string() const;
	int 					get_int() const;
	float 					get_float() const;
	double 					get_double() const;
	bool 					get_bool() const;

	bool 					key_exists(const std::string&) const;

	const dnot_token&			operator[](const std::string&) const;
	dnot_token&				operator[](const std::string&);
	const dnot_token&			operator[](const char *) const;
	dnot_token&				operator[](const char *);
	const dnot_token&			operator[](size_t) const;
	dnot_token&				operator[](size_t);
	const dnot_token&			operator[](int) const;
	dnot_token&				operator[](int);

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

//	operator				t_map& () const {return tokens;}
//	operator				t_vector& () const {return vector;}

	operator 				std::string() const {return get_string();}
	operator				int() const {return get_int();}
	operator				bool() const {return get_bool();}
	operator				float() const {return get_float();}
	operator				double() const {return get_double();}

						dnot_token();
	explicit				dnot_token(const std::string& v);
	explicit 				dnot_token(const char *);
	explicit 				dnot_token(const char);
	explicit 				dnot_token(int v);
	explicit 				dnot_token(float v);
	explicit 				dnot_token(double v);
	explicit 				dnot_token(bool v);
	explicit 				dnot_token(const t_map& v);
	explicit 				dnot_token(const t_vector& v);
	private:

	enum class types {
		tmap, 	//El token es un objeto...
		tstring,	//El token tiene un valor string
		tint,	//El token tiene un valor int
		tfloat,	//El token tiene un valor float
		tdouble,	//El token tiene un valor double
		tbool,	//El token tiene un valor bool
		tvector};		//El token es una vector de otros tokens anónimos.

	std::string				translate_type(types)const;
	std::string				to_string()const;

	types					type;
	t_map					tokens;
	t_vector				vector;
	std::string				string_value;
	int					int_value;
	float					float_value;
	double					double_value;
	bool					bool_value;

	friend class dnot_parser;
	friend std::ostream& operator<<(std::ostream& os, const tools::dnot_token& t);
};

std::ostream& operator<<(std::ostream& os, const tools::dnot_token& t);

}

#endif
