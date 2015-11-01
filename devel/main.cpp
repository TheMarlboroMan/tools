#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <stdexcept>

struct Token
{
	enum class tipos {
		compuesto, 	//El token tiene clave y el valor es un array de tokens.
		valor,		//El token tiene un valor.
		lista};		//El token es una lista de otros tokens anónimos.

	std::string 		clave;
	tipos			tipo;

	std::vector<Token>	tokens;
	std::string		valor;

	bool es_anonimo() const {return !clave.size();}
	bool es_valor() const {return tipo==tipos::valor;}
	bool es_objeto() const {return tipo==tipos::compuesto;}
	bool es_lista() const {return tipo==tipos::lista;}

	void asignar_valor(const std::string c)
	{
		valor=c;
		tipo=tipos::valor;
	}

	void asignar_tokens(const std::vector<Token>& t)
	{
		tokens=t;
		tipo=tipos::compuesto;
	}

	void asignar_lista(const std::vector<Token>& t)
	{
		tokens=t;
		tipo=tipos::lista;
	}

	Token(const std::string& c): clave(c), tipo(tipos::valor) {}
	Token(const std::string& c, const std::string& v): clave(c), valor(v), tipo(tipos::valor) {}
};

class Parser
{
	private:

	enum class tipos {objeto, lista};

	public:

	Parser(std::ifstream& fichero, tipos t=tipos::objeto)
		:estado(estados::leyendo), 
		tipo(t),
		fichero(fichero)
	{
		buffer.reserve(1024);
	}

	void operator()()
	{
		char cb;
		while(true)
		{	
			if(estado==estados::salir) 
			{
				break;
			}

			fichero>>cb;
			buffer+=cb;

			if(fichero.eof())
			{
				if(estado==estados::leyendo) asignar_valor_objeto();
				break;
			}

			switch(cb)
			{
				case ',': coma(); break;
				case '{': abre_llave(); break;
				case '}': cierra_llave(); break;
				case '[': abre_corchete(); break;
				case ']': cierra_corchete(); break;
			}
		}
	}

	std::vector<Token>& acc_tokens() {return tokens;}
	
	private:

/**
* La coma indica que se ha llegado al final de una enumeración. En función de
* si el parser es de objeto o lista llamaremos a la función de turno.
* Se contempla el caso que encontremos una coma tras ] o }, que indicaría el
* fin de un subparser. Dado que el estado de [ o { contempla la inserción
* no haríamos nada.
*/

	void coma()
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

	void abre_llave()
	{
		switch(estado)
		{
			case estados::leyendo:
			{
				Parser p(fichero);
				p();
				asignar_subparser_objeto(p.tokens);
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

	void cierra_llave()
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

	void abre_corchete()
	{
		switch(estado)
		{
			case estados::leyendo:
			{
				Parser p(fichero, tipos::lista);
				p();
				asignar_subparser_lista(p.tokens);
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

	void cierra_corchete()
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

	void asignar_valor_objeto()
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
			tokens.push_back(Token(clave, valor));
			buffer.clear();
		}
	}

/**
* Cuando estamos en un parser de modo lista no podemos tener dos puntos: una
* lista es un array de tokens anónimos. A esta función la llamaremos tras
* encontrar una coma en un parser de lista.
*/

	void asignar_valor_lista()
	{
		size_t pos=buffer.find(":");
		if(pos!=std::string::npos)
		{
			error("Se encuentra : asignando valor a lista.");
		}

		std::string valor=buffer.substr(0, buffer.size()-1);
		tokens.push_back(Token("", valor));
		buffer.clear();
	}

/**
* Se asigna el array de tokens como "grupo" o "objeto" del token actual. Se
* contempla el caso de un token anónimo (para crear un objeto dentro de una 
* lista.
*/

	void asignar_subparser_objeto(const std::vector<Token>& aux)
	{
		Token T=obtener_token_para_subparser();
		T.asignar_tokens(aux);
		tokens.push_back(T);
	}

/*
* Se asigna el array de tokens (supuestamente todos son anónimos) como 
* lista para el token actual al cerrar un corchete. Al igual que en el caso
* anterior, el token puede o no ser anónimo.
*/

	void asignar_subparser_lista(const std::vector<Token>& aux)
	{
		Token T=obtener_token_para_subparser();
		T.asignar_lista(aux);
		tokens.push_back(T);
	}

/*
* Se genera un token, anónimo o no.
*/
	Token obtener_token_para_subparser()
	{
		size_t pos=buffer.find(":");
		std::string clave;
		if(pos!=std::string::npos)
		{
			clave=buffer.substr(0, pos);
		}
		return Token(clave);
	}

	void error(const std::string& msj)
	{
		throw std::runtime_error(msj+"\nESTADO:"+traducir_estado()+"\nBUFFER:"+buffer);
	}

	std::string traducir_estado()
	{
		switch(estado)
		{
			case estados::leyendo: return "LEYENDO"; break;
			case estados::fin_subparser: return "FIN SUBPARSER"; break;
			case estados::salir: return "SALIR"; break;
		}
	}	

	enum class estados {leyendo, salir, fin_subparser};

	estados			estado;
	tipos			tipo;
	std::ifstream&		fichero;
	std::string 		buffer;
	std::vector<Token>	tokens;
};

/*****************************************/

void dime_token(const Token& t, int tabs=0)
{
	std::string ver_tabs;
	for(int i=0; i<tabs; ++i) ver_tabs+="\t";

	std::cout<<ver_tabs;
	if(!t.es_anonimo()) std::cout<<t.clave<<":";

	if(t.es_valor())
	{
		std::cout<<t.valor<<std::endl;
	}
	else if(t.es_objeto())
	{
		std::cout<<ver_tabs<<"{"<<std::endl;
		for(const auto& tok : t.tokens)
			dime_token(tok, tabs+1);
		std::cout<<ver_tabs<<"}"<<std::endl;
	}
	else if(t.es_lista())
	{
		std::cout<<ver_tabs<<"["<<std::endl;
		for(const auto& tok : t.tokens)
			dime_token(tok, tabs+1);	
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

		Parser p(fichero);
		p();

		const auto& tokens=p.acc_tokens();
		
		for(const auto& t : tokens)
		{
			dime_token(t);
		}

		return 0;
	}
	catch(std::exception& e)
	{
		std::cout<<"ERROR : "<<e.what();
		return 1;
	}
}
