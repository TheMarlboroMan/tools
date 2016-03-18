#include "dnot_token.h"

#ifdef WINCOMPIL
/* Localización del parche mingw32... Esto debería estar en otro lado, supongo. */
#include <herramientas/herramientas/herramientas.h>
#endif

using namespace Herramientas_proyecto;

bool Dnot_token::es_valor() const 
{
	return tipo==tipos::valor_string ||
	tipo==tipos::valor_int ||
	tipo==tipos::valor_float ||
	tipo==tipos::valor_bool ||
	tipo==tipos::valor_double;
}



void Dnot_token::asignar(const std::string c)
{
	valor_string=c;
	tipo=tipos::valor_string;
}

void Dnot_token::asignar(const char * c)
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

void Dnot_token::asignar(double c)
{
	valor_double=c;
	tipo=tipos::valor_double;
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
	valor_string(""), valor_int(0), valor_float(0.f), valor_double(0.0), valor_bool(false)
{

}

Dnot_token::Dnot_token(const std::string& v)
	:tipo(tipos::valor_string), 
	valor_string(v), valor_int(0), valor_float(0.f), valor_double(0.0), valor_bool(false)
{

}

Dnot_token::Dnot_token(const char * v)
	:tipo(tipos::valor_string), 
	valor_string(v), valor_int(0), valor_float(0.f), valor_double(0.0), valor_bool(false)
{

}

Dnot_token::Dnot_token(const char v)
	:tipo(tipos::valor_string), 
	valor_string(), valor_int(0), valor_float(0.f), valor_double(0.0), valor_bool(false)
{
	valor_string+=v;
}

Dnot_token::Dnot_token(int v)
	:tipo(tipos::valor_int),
	valor_string(""), valor_int(v), valor_float(0.f), valor_double(0.0), valor_bool(false)
{

}

Dnot_token::Dnot_token(float v)
	:tipo(tipos::valor_float),
	valor_string(""), valor_int(0), valor_float(v), valor_double(0.0), valor_bool(false)
{

}

Dnot_token::Dnot_token(double v)
	:tipo(tipos::valor_double),
	valor_string(""), valor_int(0), valor_float(0.0f), valor_double(v), valor_bool(false)
{

}

Dnot_token::Dnot_token(bool v)
	:tipo(tipos::valor_bool),
	valor_string(""), valor_int(0), valor_float(0.f), valor_double(0.0), valor_bool(v)
{

}

Dnot_token::Dnot_token(const t_mapa& v)
	:tipo(tipos::compuesto),
	valor_string(""), valor_int(0), valor_float(0.f), valor_double(0.0), valor_bool(false)
{
	asignar(v);
}

Dnot_token::Dnot_token(const t_vector& v)
	:tipo(tipos::lista),
	valor_string(""), valor_int(0), valor_float(0.f), valor_double(0.0), valor_bool(false)
{
	asignar(v);
}

const Dnot_token::t_mapa& Dnot_token::acc_tokens() const 
{
	if(tipo!=tipos::compuesto) throw std::runtime_error("El tipo no es compuesto ["+traducir_tipo(tipo)+" - "+valor_como_string()+"]");
	return tokens;
}

Dnot_token::t_mapa& Dnot_token::acc_tokens()
{
	if(tipo!=tipos::compuesto) throw std::runtime_error("El tipo no es compuesto ["+traducir_tipo(tipo)+" - "+valor_como_string()+"]");
	return tokens;
}

const Dnot_token::t_vector& Dnot_token::acc_lista() const 
{
	if(tipo!=tipos::lista) throw std::runtime_error("El tipo no es lista ["+traducir_tipo(tipo)+" - "+valor_como_string()+"]");
	return lista;
}

Dnot_token::t_vector& Dnot_token::acc_lista()
{
	if(tipo!=tipos::lista) throw std::runtime_error("El tipo no es lista ["+traducir_tipo(tipo)+" - "+valor_como_string()+"]");
	return lista;
}

const std::string& Dnot_token::acc_string() const 
{
	if(tipo!=tipos::valor_string) throw std::runtime_error("El tipo no es string ["+traducir_tipo(tipo)+" - "+valor_como_string()+"]");
	return valor_string;
}

int Dnot_token::acc_int() const 
{
	if(tipo!=tipos::valor_int) throw std::runtime_error("El tipo no es int ["+traducir_tipo(tipo)+" - "+valor_como_string()+"]");
	return valor_int;
}

float Dnot_token::acc_float() const 
{
	if(tipo!=tipos::valor_float) throw std::runtime_error("El tipo no es float ["+traducir_tipo(tipo)+" - "+valor_como_string()+"]");
	return valor_float;
}

double Dnot_token::acc_double() const 
{
	if(tipo!=tipos::valor_double) throw std::runtime_error("El tipo no es double ["+traducir_tipo(tipo)+" - "+valor_como_string()+"]");
	return valor_double;
}

bool Dnot_token::acc_bool() const 
{
	if(tipo!=tipos::valor_bool) throw std::runtime_error("El tipo no es bool ["+traducir_tipo(tipo)+" - "+valor_como_string()+"]");
	return valor_bool;
}

const Dnot_token& Dnot_token::operator[](const std::string& k) const
{
	if(tipo!=tipos::compuesto) throw std::runtime_error("El tipo no es compuesto ["+traducir_tipo(tipo)+" - "+valor_como_string()+"]");
	else if(!tokens.count(k)) throw std::runtime_error("La clave "+k+" no existe");
	else return tokens.at(k);
}

Dnot_token& Dnot_token::operator[](const std::string& k)
{
	if(tipo!=tipos::compuesto) throw std::runtime_error("El tipo no es compuesto ["+traducir_tipo(tipo)+" - "+valor_como_string()+"]");
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
	if(tipo!=tipos::lista) throw std::runtime_error("El tipo no es lista ["+traducir_tipo(tipo)+" - "+valor_como_string()+"]");
	else if(l >= lista.size()) throw std::runtime_error("El indice de lista es inválido");
	else return lista.at(l);
}

Dnot_token& Dnot_token::operator[](size_t l)
{
	if(tipo!=tipos::lista) throw std::runtime_error("El tipo no es lista ["+traducir_tipo(tipo)+" - "+valor_como_string()+"]");
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
		case Dnot_token::tipos::valor_double: os<<t.valor_double; break;
		case Dnot_token::tipos::valor_bool: t.valor_bool ? os<<"true" : os<<"false"; break;
	}

	return os;
}

bool Dnot_token::existe_clave(const std::string& k) const
{
	if(tipo!=tipos::compuesto) throw std::runtime_error("El tipo no es compuesto ["+traducir_tipo(tipo)+" - "+valor_como_string()+"]");
	else return tokens.count(k);
}

std::string Dnot_token::serializar(const Dnot_token_opciones_serializador& opciones, int recursividad) const
{
	std::string resultado;

#ifdef WINCOMPIL
	using namespace parche_mingw;
#else
	using namespace std;
#endif

	size_t tot=0, cur=0;

	auto nl=[&resultado, &opciones](int rec)
	{
		resultado+="\n";
		for(int i=0; i<rec; ++i) 
		{
			resultado+=opciones.tabulador;
		}
	};


	auto abre=[&resultado, &nl, &opciones](tipos tipo, int rec)
	{
		if(rec < 1) return;
		resultado+=tipo==tipos::lista ? "[" : "{";
		if(opciones.tabular_profundidad) nl(rec);
	};

	auto cierra=[&resultado, &nl, &opciones](tipos tipo, int rec)
	{
		if(rec < 1) return;
		if(opciones.tabular_profundidad) nl(rec-1);
		resultado+=tipo==tipos::lista ? "]" : "}";
	};

	switch(tipo)
	{
		case tipos::lista: 
		case tipos::compuesto:

			cur=0;
			tot=tipo==tipos::lista ? lista.size()-1 : tokens.size()-1;

			abre(tipo, recursividad);
			if(tipo==tipos::lista)
			{
				for(const auto& e : lista) 
				{
					resultado+=e.serializar(opciones, recursividad+1);
					if(cur++!=tot) resultado+=", ";
				}
			}
			else if(tipo==tipos::compuesto)
			{
				for(const auto& e : tokens) 
				{
					resultado+=e.first+":"+e.second.serializar(opciones, recursividad+1);
					if(cur++!=tot) resultado+=", ";
				}
			}
			cierra(tipo, recursividad);

		break;
		case tipos::valor_string: 
			resultado+="\""+valor_string+"\"";
		break;
		case tipos::valor_int: 
			resultado+=to_string(valor_int);
		break;
		case tipos::valor_float:
			resultado+=to_string(valor_float)+'f';
		break;
		case tipos::valor_double:
			resultado+=to_string(valor_double);
		break;
		case tipos::valor_bool:
			resultado+=valor_bool ? "true" : "false";
		break;
	}

	return resultado;
}

std::string Dnot_token::traducir_tipo(Dnot_token::tipos t)const
{
	switch(t)
	{
		case tipos::compuesto: return "compuesto"; break;
		case tipos::valor_string: return "string"; break;
		case tipos::valor_int: return "int"; break;
		case tipos::valor_float: return "float"; break;
		case tipos::valor_double: return "double"; break;
		case tipos::valor_bool: return "bool"; break;
		case tipos::lista:	return "lista"; break;
		default: return "desconocido"; break;
	}
	
}

std::string Dnot_token::valor_como_string()const
{
#ifdef WINCOMPIL
	using namespace parche_mingw;
#else
	using namespace std;
#endif

	switch(tipo)
	{
		case tipos::compuesto: return "compuesto ("+to_string(tokens.size())+")"; break;
		case tipos::valor_string: return valor_string; break;
		case tipos::valor_int: return to_string(valor_int); break;
		case tipos::valor_float: return to_string(valor_float); break;
		case tipos::valor_double: return to_string(valor_double); break;
		case tipos::valor_bool: return valor_bool ? "true" : "false"; break;
		case tipos::lista:	return "lista ("+to_string(lista.size())+")"; break;
		default: return "** desconocido **"; break;
	}
}
