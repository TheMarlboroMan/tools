#include "dnot_parser.h"

using namespace Herramientas_proyecto;

Dnot_parser::Dnot_parser(std::istream& stream, tipos t)
	:estado(estados::leyendo), 
	leer_comillas(false), finalizado(false),
	tipo(t),
	stream(stream)
{
	buffer.reserve(1024);
	token.tipo=Dnot_token::tipos::compuesto;
}

void Dnot_parser::operator()()
{
	while(true)
	{	
		if(estado==estados::salir) 
		{
			break;
		}

		//Tenemos dos modos de lectura: estoy "dentro de una string"
		//o fuera.

		char cb=leer_comillas ? leer_string() : leer_stream();

		if(stream.eof())
		{
			//TODO: Es posible que esté malformado si estamos leyendo comillas;
			if(estado==estados::leyendo) asignar_valor_objeto();
			estado=estados::salir;				
			finalizado=true;
			break;
		}

		buffer+=cb;
		leer_comillas ? procesar_string(cb) : procesar_stream(cb);
	}
}

/**
* Lee lo que sea que haya en el stream, sea o no whitespace.
*/

char Dnot_parser::leer_string()
{
	char cb;
	stream.get(cb);
	return cb;
}

/**
* Lee el stream ignorando whitespace.
*/

char Dnot_parser::leer_stream()
{
	char cb;
	stream>>cb;
	return cb;
}

/*
* Procesa el carácter leido en modo string. Se guarda lo que sea que tenga a 
* no ser que encuentre unas comillas, que finaliza el modo de lectura
* entrecomillada.
* TODO: No es posible escapar comillas dentro de una secuencia de comillas.
* Podríamos usar una secuencia de escape para leer, simplemente si encontramos
* el carácter \ lo siguiente entraría en el buffer tal cual.
*/

void Dnot_parser::procesar_string(char cb)
{
	if(cb=='"') comillas();
}

/*
* Procesa el carácter leido en modo stream, lo que indica que si lee algún
* carácter de control le asignará su significado.
*/

void Dnot_parser::procesar_stream(char cb)
{
	switch(cb)
	{
		case ',': coma(); break;
		case '"': comillas(); break;
		case '{': abre_llave(); break;
		case '}': cierra_llave(); break;
		case '[': abre_corchete(); break;
		case ']': cierra_corchete(); break;
	}
}

/**
* Indica que ha leido el carácter de control ", que abre o cierra una secuencia
* de comillas.
*/

void Dnot_parser::comillas()
{
	leer_comillas=!leer_comillas;
}

/**
* La coma indica que se ha llegado al final de una enumeración. En función de
* si el parser es de objeto o lista llamaremos a la función de turno.
* Se contempla el caso que encontremos una coma tras ] o }, que indicaría el
* fin de un subparser. Dado que el estado de [ o { contempla la inserción
* no haríamos nada.
*/

void Dnot_parser::coma()
{
	switch(estado)
	{	
		case estados::leyendo:
			switch(tipo)
			{
				case tipos::objeto: asignar_valor_objeto(); break;
				case tipos::lista: asignar_valor_lista(); break;
			}
		break;
		case estados::fin_subparser:
			//NOOP;
		break;
		default:	
			error("Encontrado , en estado incorrecto");
		break;
	}

	estado=estados::leyendo;
	buffer.clear();
}

/**
* Crea un subparser en modo objeto y asigna los tokens leidos del mismo. El
* estado final es "fin_subparser", que indica que la siguiente coma debe 
* ignorarse con respecto a la inserción.
*/

void Dnot_parser::abre_llave()
{
	switch(estado)
	{
		case estados::leyendo:
		{
			Dnot_parser p(stream);
			p();
			asignar_subparser_objeto(p.token.tokens);
			estado=estados::fin_subparser;
		}
		break;

		default:
			error("Encontrado { en estado incorrecto : ");
		break;
	}
}

/*
* Eso se hará desde dentro de un subparser y asignará el último valor, como
* si fuera una coma. Luego marcará el subparser como "finalizado".
*/

void Dnot_parser::cierra_llave()
{
	switch(estado)
	{
		case estados::leyendo:
			asignar_valor_objeto();
		break;
		case estados::fin_subparser:
			//NOOP.
		break;
		default:
			error("Encontrado } en estado incorrecto");
		break;
	}

	estado=estados::salir;
	buffer.clear();
}

/**
* Idem que abre llave.
*/

void Dnot_parser::abre_corchete()
{
	switch(estado)
	{
		case estados::leyendo:
		{
			Dnot_parser p(stream, tipos::lista);
			p();
			asignar_subparser_lista(p.token.lista);
			estado=estados::fin_subparser;
		}
		break;

		default:
			error("Encontrado [ en estado incorrecto : ");
		break;
	}
}

/**
* Idem que "cierra llave.
*/

void Dnot_parser::cierra_corchete()
{
	switch(estado)
	{
		case estados::leyendo:
			asignar_valor_lista();
		break;
		case estados::fin_subparser:
			//NOOP.
		break;
		default:
			error("Encontrado ] en estado incorrecto");
		break;
	}

	estado=estados::salir;
	buffer.clear();
}

/**
* Se asigna un valor textual a un token, ya sea por encontrar una coma o por
* llegarse al fin del fichero. Se partirá el buffer en dos para separar la
* clave del valor.
*/

void Dnot_parser::asignar_valor_objeto()
{
	size_t pos=buffer.find(":");
	if(pos==std::string::npos)
	{
		error("No se localizan : para objeto");
	}
	else
	{
		std::string clave=buffer.substr(0, pos);
		std::string valor=buffer.substr(pos+1, buffer.size()-clave.size()-2);

		if(token.tokens.count(clave))
		{
			error("La clave "+clave+" ya existe para token");
		}

		token.tipo=Dnot_token::tipos::compuesto;
		token.tokens[clave]=generar_token_valor(valor);
		buffer.clear();
	}
}

/*
* Infiere el tipo de valor y lo asigna a un token que devuelve. Las normas de 
* inferencia son bastante naive, pero de momento nos valen así. Ojo, los 
* operadores de asignación del token son explícitos, de modo que necesitamos el 
* tipo de dato exacto.
*/ 

Dnot_token Dnot_parser::generar_token_valor(const std::string& v)
{
	Dnot_token t;

	//Comprobar bool
	if(v=="true" || v=="false")
	{			
		t.asignar(v=="true");
	}
	//String...
	else if(v.front()=='"' && v.back()=='"')
	{
		t.asignar(v.substr(1, v.size()-2));
	}
	//Float y double...
	else if(v.find(".") != std::string::npos)
	{
		if(v.back()=='f')
		{
			t.asignar((float)std::atof(v.c_str()));
		}
		else
		{
			t.asignar(std::atof(v.c_str()));
		}
	}
	//Entero...
	else if(std::all_of(std::begin(v), std::end(v), [](const char c) {return isdigit(c) || c=='-';}))
	{
		t.asignar(std::atoi(v.c_str()));
	}
	else
	{
		error("Imposible inferir valor");
	}

	return t;
}

/**
* Cuando estamos en un parser de modo lista no podemos tener dos puntos: una
* lista es un array de tokens anónimos. A esta función la llamaremos tras
* encontrar una coma en un parser de lista.
*/

void Dnot_parser::asignar_valor_lista()
{
	size_t pos=buffer.find(":");
	if(pos!=std::string::npos)
	{
		error("Se encuentra : asignando valor a lista.");
	}

	std::string valor=buffer.substr(0, buffer.size()-1);
	token.tipo=Dnot_token::tipos::lista;
	if(valor.size()) token.lista.push_back(generar_token_valor(valor));
	buffer.clear();
}

/**
* Se asigna el array de tokens como "grupo" o "objeto" del token actual. Se
* contempla el caso de un token anónimo (para crear un objeto dentro de una 
* lista.
*/

void Dnot_parser::asignar_subparser_objeto(const std::map<std::string, Dnot_token>& aux)
{
	Dnot_token T;
	T.asignar(aux);
	size_t pos=buffer.find(":");
	if(pos==std::string::npos)
	{
		token.lista.push_back(T);
	}
	else
	{
		token.tokens[buffer.substr(0, pos)]=T;
	}
}

/*
* Se asigna el array de tokens (supuestamente todos son anónimos) como 
* lista para el token actual al cerrar un corchete. Al igual que en el caso
* anterior, el token puede o no ser anónimo.
*/

void Dnot_parser::asignar_subparser_lista(const std::vector<Dnot_token>& aux)
{
	//TODO: Esto falla: no podemos asignar listas. El buffer sigue lleno
	//cuando llegamos a este punto. se asigna la lista del token pero
	//el token no tiene nombre!!!. No tenemos una estructura capaz de 
	//representar eso.

	Dnot_token T;
	T.asignar(aux);

	size_t pos=buffer.find(":");
	//Si el token es anónimo simplemente insertamos algo en la lista...
	if(pos==std::string::npos)
	{
		token.lista.push_back(T);
	}
	else
	{
		token.tokens[buffer.substr(0, pos)]=T;
	}
}

void Dnot_parser::error(const std::string& msj)
{
	throw std::runtime_error(msj+"\nESTADO:"+traducir_estado()+"\nBUFFER:"+buffer);
}

std::string Dnot_parser::traducir_estado()
{
	switch(estado)
	{
		case estados::leyendo: return "LEYENDO"; break;
		case estados::fin_subparser: return "FIN SUBPARSER"; break;		
		case estados::salir: return "SALIR"; break;
		default: return "DESCONOCIDO"; break;
	}
}

Dnot_token Herramientas_proyecto::parsear_dnot(const std::string& c)
{
	std::ifstream f(c.c_str());
	if(!f.is_open()) 
	{
		throw std::runtime_error("Imposible abrir fichero "+c);
	}
	Dnot_parser p(f);
	p();
	return p.acc_token();
}
