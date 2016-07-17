#include "dnot_parser.h"

using namespace tools;

dnot_parser::dnot_parser(std::istream& stream, types t)
	:estado(tstates::reading), 
	read_quotes(false), done(false),
	type(t),
	stream(stream)
{
	buffer.reserve(1024);
	token.type=dnot_token::types::tmap;
}

void dnot_parser::operator()()
{
	while(true)
	{	
		if(estado==tstates::exiting) 
		{
			break;
		}

		//Tenemos dos modos de lectura: estoy "dentro de una string"
		//o fuera.

		char cb=read_quotes ? read_string() : read_stream();

		if(stream.eof())
		{
			//TODO: Es posible que esté malformado si estamos reading quotes;
			if(estado==tstates::reading) assign_tobject();
			estado=tstates::exiting;				
			done=true;
			break;
		}

		buffer+=cb;
		read_quotes ? process_string(cb) : process_stream(cb);
	}
}

/**
* Lee lo que sea que haya en el stream, sea o no whitespace.
*/

char dnot_parser::read_string()
{
	char cb;
	stream.get(cb);
	return cb;
}

/**
* Lee el stream ignorando whitespace.
*/

char dnot_parser::read_stream()
{
	char cb;
	stream>>cb;
	return cb;
}

/*
* Procesa el carácter leido en modo string. Se guarda lo que sea que tenga a 
* no ser que encuentre unas quotes, que finaliza el modo de lectura
* entrecomillada.
* TODO: No es posible escapar quotes dentro de una secuencia de quotes.
* Podríamos usar una secuencia de escape para leer, simplemente si encontramos
* el carácter \ lo siguiente entraría en el buffer tal cual.
*/

void dnot_parser::process_string(char cb)
{
	if(cb=='"') quotes();
}

/*
* Procesa el carácter leido en modo stream, lo que indica que si lee algún
* carácter de control le assigná su significado.
*/

void dnot_parser::process_stream(char cb)
{
	switch(cb)
	{
		case ',': comma(); break;
		case '"': quotes(); break;
		case '{': open_brace(); break;
		case '}': close_brace(); break;
		case '[': open_bracket(); break;
		case ']': close_bracket(); break;
	}
}

/**
* Indica que ha leido el carácter de control ", que abre o cierra una secuencia
* de quotes.
*/

void dnot_parser::quotes()
{
	read_quotes=!read_quotes;
}

/**
* La comma indica que se ha llegado al final de una enumeración. En función de
* si el parser es de tobject o tarray llamaremos a la función de turno.
* Se contempla el caso que encontremos una comma tras ] o }, que indicaría el
* fin de un subparser. Dado que el estado de [ o { contempla la inserción
* no haríamos nada.
*/

void dnot_parser::comma()
{
	switch(estado)
	{	
		case tstates::reading:
			switch(type)
			{
				case types::tmap: assign_tobject(); break;
				case types::tvector: assign_tarray(); break;
			}
		break;
		case tstates::exiting_subparser:
			//NOOP;
		break;
		default:	
			error("found comma in invalid state");
		break;
	}

	estado=tstates::reading;
	buffer.clear();
}

/**
* Crea un subparser en modo tobject y asigna los tokens leidos del mismo. El
* estado final es "exiting_subparser", que indica que la siguiente comma debe 
* ignorarse con respecto a la inserción.
*/

void dnot_parser::open_brace()
{
	switch(estado)
	{
		case tstates::reading:
		{
			dnot_parser p(stream);
			p();
			assign_tobject_subparser(p.token.tokens);
			estado=tstates::exiting_subparser;
		}
		break;

		default:
			error("found open brace in incorrect state : ");
		break;
	}
}

/*
* Eso se hará desde dentro de un subparser y assigná el último valor, como
* si fuera una comma. Luego marcará el subparser como "done".
*/

void dnot_parser::close_brace()
{
	switch(estado)
	{
		case tstates::reading:
			assign_tobject();
		break;
		case tstates::exiting_subparser:
			//NOOP.
		break;
		default:
			error("found close brace in incorrect state");
		break;
	}

	estado=tstates::exiting;
	buffer.clear();
}

/**
* Idem que abre llave.
*/

void dnot_parser::open_bracket()
{
	switch(estado)
	{
		case tstates::reading:
		{
			dnot_parser p(stream, types::tvector);
			p();
			assign_tarray_subparser(p.token.vector);
			estado=tstates::exiting_subparser;
		}
		break;

		default:
			error("found open bracket in incorrect state : ");
		break;
	}
}

/**
* Idem que "cierra llave.
*/

void dnot_parser::close_bracket()
{
	switch(estado)
	{
		case tstates::reading:
			assign_tarray();
		break;
		case tstates::exiting_subparser:
			//NOOP.
		break;
		default:
			error("found close bracket in incorrect state");
		break;
	}

	estado=tstates::exiting;
	buffer.clear();
}

/**
* Se asigna un valor textual a un token, ya sea por encontrar una comma o por
* llegarse al fin del fichero. Se partirá el buffer en dos para separar la
* clave del valor.
*/

void dnot_parser::assign_tobject()
{
	size_t pos=buffer.find(":");
	if(pos==std::string::npos)
	{
		error("no color for object found");
	}
	else
	{
		std::string clave=buffer.substr(0, pos);
		std::string valor=buffer.substr(pos+1, buffer.size()-clave.size()-2);

		if(token.tokens.count(clave))
		{
			error("repeated key "+clave+" for object");
		}

		token.type=dnot_token::types::tmap;
		token.tokens[clave]=generate_token(valor);
		buffer.clear();
	}
}

/*
* Infiere el type de valor y lo asigna a un token que devuelve. Las normas de 
* inferencia son bastante naive, pero de momento nos valen así. Ojo, los 
* operadores de asignación del token son explícitos, de modo que necesitamos el 
* type de dato exacto.
*/ 

dnot_token dnot_parser::generate_token(const std::string& v)
{
	dnot_token t;

	//Comprobar bool
	if(v=="true" || v=="false")
	{			
		t.assign(v=="true");
	}
	//String...
	else if(v.front()=='"' && v.back()=='"')
	{
		t.assign(v.substr(1, v.size()-2));
	}
	//Float y double...
	else if(v.find(".") != std::string::npos)
	{
		if(v.back()=='f')
		{
			t.assign((float)std::atof(v.c_str()));
		}
		else
		{
			t.assign(std::atof(v.c_str()));
		}
	}
	//Entero...
	else if(std::all_of(std::begin(v), std::end(v), [](const char c) {return isdigit(c) || c=='-';}))
	{
		t.assign(std::atoi(v.c_str()));
	}
	else
	{
		error("unable to infer value");
	}

	return t;
}

/**
* Cuando estamos en un parser de modo tarray no podemos tener dos puntos: una
* tarray es un array de tokens anónimos. A esta función la llamaremos tras
* encontrar una comma en un parser de tarray.
*/

void dnot_parser::assign_tarray()
{
	size_t pos=buffer.find(":");
	if(pos!=std::string::npos)
	{
		error("colon found in array");
	}

	std::string valor=buffer.substr(0, buffer.size()-1);
	token.type=dnot_token::types::tvector;
	if(valor.size()) token.vector.push_back(generate_token(valor));
	buffer.clear();
}

/**
* Se asigna el array de tokens como "grupo" o "tobject" del token actual. Se
* contempla el caso de un token anónimo (para crear un tobject dentro de una 
* tarray.
*/

void dnot_parser::assign_tobject_subparser(const std::map<std::string, dnot_token>& aux)
{
	dnot_token T;
	T.assign(aux);
	size_t pos=buffer.find(":");
	if(pos==std::string::npos)
	{
		token.vector.push_back(T);
	}
	else
	{
		token.tokens[buffer.substr(0, pos)]=T;
	}
}

/*
* Se asigna el array de tokens (supuestamente todos son anónimos) como 
* tarray para el token actual al cerrar un corchete. Al igual que en el caso
* anterior, el token puede o no ser anónimo.
*/

void dnot_parser::assign_tarray_subparser(const std::vector<dnot_token>& aux)
{
	//TODO: Esto falla: no podemos assign tarrays. El buffer sigue lleno
	//cuando llegamos a este punto. se asigna la tarray del token pero
	//el token no tiene nombre!!!. No tenemos una estructura capaz de 
	//representar eso.

	dnot_token T;
	T.assign(aux);

	size_t pos=buffer.find(":");
	//Si el token es anónimo simplemente insertamos algo en la tarray...
	if(pos==std::string::npos)
	{
		token.vector.push_back(T);
	}
	else
	{
		token.tokens[buffer.substr(0, pos)]=T;
	}
}

void dnot_parser::error(const std::string& msj)
{
	throw std::runtime_error(msj+"\nstate:"+get_state()+"\nbuffer:"+buffer);
}

std::string dnot_parser::get_state()
{
	switch(estado)
	{
		case tstates::reading: return "reading"; break;
		case tstates::exiting_subparser: return "exit subparser"; break;		
		case tstates::exiting: return "exit"; break;
	}

	//Shut compiler up.
	return std::string();
}

dnot_token tools::dnot_parse(const std::string& c)
{
	std::ifstream f(c.c_str());
	if(!f.is_open()) 
	{
		throw std::runtime_error("unable to parse file "+c);
	}

	dnot_parser p(f);
	p();
	return p.get_token();
}
