#include "dnot_token.h"

using namespace Herramientas_proyecto;

bool Dnot_token::es_valor() const 
{
	return tipo==tipos::valor_string ||
	tipo==tipos::valor_int ||
	tipo==tipos::valor_float ||
	tipo==tipos::valor_bool;
}

void Dnot_token::asignar(const std::string c)
{
	valor_string=c;
	tipo=tipos::valor_string;
}

void Dnot_token::asignar(int c)
{
	valor_int=c;
	tipo=tipos::valor_int;
}

void Dnot_token::asignar(float c)
{
	valor_float=c;
	tipo=tipos::valor_float;
}

void Dnot_token::asignar(bool c)
{
	valor_bool=c;
	tipo=tipos::valor_bool;
}

void Dnot_token::asignar(const t_mapa& t)
{
	tokens=t;
	tipo=tipos::compuesto;
}

void Dnot_token::asignar(const t_vector& t)
{
	lista=t;
	tipo=tipos::lista;
}

Dnot_token::Dnot_token()
	:tipo(tipos::valor_string),
	valor_string(""), valor_int(0), valor_float(0.f), valor_bool(false)
{

}

Dnot_token::Dnot_token(const std::string& v)
	:tipo(tipos::valor_string), 
	valor_string(v), valor_int(0), valor_float(0.f), valor_bool(false)
{

}

Dnot_token::Dnot_token(int v)
	:tipo(tipos::valor_int),
	valor_string(""), valor_int(v), valor_float(0.f), valor_bool(false)
{

}

Dnot_token::Dnot_token(float v)
	:tipo(tipos::valor_float),
	valor_string(""), valor_int(0), valor_float(v), valor_bool(false)
{

}

Dnot_token::Dnot_token(bool v)
	:tipo(tipos::valor_bool),
	valor_string(""), valor_int(0), valor_float(0.f), valor_bool(v)
{

}

const Dnot_token::t_mapa& Dnot_token::acc_tokens() const 
{
	if(tipo!=tipos::compuesto) throw std::runtime_error("El tipo no es compuesto");
	return tokens;
}

Dnot_token::t_mapa& Dnot_token::acc_tokens()
{
	if(tipo!=tipos::compuesto) throw std::runtime_error("El tipo no es compuesto");
	return tokens;
}

const Dnot_token::t_vector& Dnot_token::acc_lista() const 
{
	if(tipo!=tipos::lista) throw std::runtime_error("El tipo no es lista");
	return lista;
}

Dnot_token::t_vector& Dnot_token::acc_lista()
{
	if(tipo!=tipos::lista) throw std::runtime_error("El tipo no es lista");
	return lista;
}

const std::string& Dnot_token::acc_string() const 
{
	if(tipo!=tipos::valor_string) throw std::runtime_error("El tipo no es string");
	return valor_string;
}

int Dnot_token::acc_int() const 
{
	if(tipo!=tipos::valor_int) throw std::runtime_error("El tipo no es int");
	return valor_int;
}

float Dnot_token::acc_float() const 
{
	if(tipo!=tipos::valor_float) throw std::runtime_error("El tipo no es float");
	return valor_float;
}

bool Dnot_token::acc_bool() const 
{
	if(tipo!=tipos::valor_bool) throw std::runtime_error("El tipo no es bool");
	return valor_bool;
}

const Dnot_token& Dnot_token::operator[](const std::string& k) const
{
	if(tipo!=tipos::compuesto) throw std::runtime_error("El tipo no es compuesto");
	else if(!tokens.count(k)) throw std::runtime_error("La clave "+k+" no existe");
	else return tokens.at(k);
}

Dnot_token& Dnot_token::operator[](const std::string& k)
{
	if(tipo!=tipos::compuesto) throw std::runtime_error("El tipo no es compuesto");
	else if(!tokens.count(k)) throw std::runtime_error("La clave "+k+" no existe");
	else return tokens[k];
}

const Dnot_token& Dnot_token::operator[](const char * k) const
{
	return this->operator[](std::string(k));
}

Dnot_token& Dnot_token::operator[](const char * k)
{
	return this->operator[](std::string(k));
}

const Dnot_token& Dnot_token::operator[](size_t l) const
{
	if(tipo!=tipos::lista) throw std::runtime_error("El tipo no es lista");
	else if(l >= lista.size()) throw std::runtime_error("El indice de lista es inválido");
	else return lista.at(l);
}

Dnot_token& Dnot_token::operator[](size_t l)
{
	if(tipo!=tipos::lista) throw std::runtime_error("El tipo no es lista");
	else if(l >= lista.size()) throw std::runtime_error("El indice de lista es inválido");
	else return lista[l];
}

const Dnot_token& Dnot_token::operator[](int l) const
{
	return this->operator[]((size_t)l);
}

Dnot_token& Dnot_token::operator[](int l)
{
	return this->operator[]((size_t)l);
}

std::ostream& Herramientas_proyecto::operator<<(std::ostream& os, const Herramientas_proyecto::Dnot_token& t)
{
	using namespace Herramientas_proyecto;

	switch(t.tipo)
	{
		case Dnot_token::tipos::compuesto: os<<"{compuesto}"; break;
		case Dnot_token::tipos::lista: os<<"[lista]"; break;
		case Dnot_token::tipos::valor_string: os<<t.valor_string; break;
		case Dnot_token::tipos::valor_int: os<<t.valor_int; break;
		case Dnot_token::tipos::valor_float: os<<t.valor_float; break;
		case Dnot_token::tipos::valor_bool: t.valor_bool ? os<<"true" : os<<"false"; break;
	}

	return os;
}
