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

void Dnot_token::asignar(const std::map<std::string, Dnot_token>& t)
{
	tokens=t;
	tipo=tipos::compuesto;
}

void Dnot_token::asignar(const std::vector<Dnot_token>& t)
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
