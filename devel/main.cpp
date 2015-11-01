#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <stdexcept>

struct Token
{
	enum class tipos {
		compuesto, 	//El token tiene clave y el valor es un array de tokens.
		lista, 		//El token es un array de tokens.
		valor, 		//El token tiene una clave y un valor.
		valor_anonimo};	//El token NO tiene clave, sólo valor.

	std::string 		clave;
	tipos			tipo;

	std::vector<Token>	tokens;
	std::vector<Token>	lista;

	std::string		valor;

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

	Token(const std::string& c): clave(c), tipo(tipos::valor) {}
};

class Parser
{
	public:

	Parser(std::ifstream& fichero)
		:estado(estados::leyendo_clave), 
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
//				std::cout<<"ABANDONANDO PARSER"<<std::endl;
				break;
			}

			fichero>>cb;
			buffer+=cb;

			if(fichero.eof())
			{
				cierra_llave(); //Es equivalente...
//				std::cout<<"FIN DE FICHERO"<<std::endl;
				break;
			}

			switch(cb)
			{
//TODO: quizás no queremos los dos puntos aún...
				case ':': dos_puntos(); break;
				case ',': coma(); break;
				case '{': abre_llave(); break;
				case '}': cierra_llave(); break;
				case '[': abre_corchete(); break;
				case ']': cierra_corchete(); break;
			}
		}
	}

	std::vector<Token>&		acc_tokens() {return tokens;}
	
	private:

	void dos_puntos()
	{
		switch(estado)
		{
			case estados::leyendo_clave:
			{
				Token T(buffer.substr(0, buffer.size()-1));
//std::cout<<"CREANDO EL TOKEN "<<T.clave<<std::endl;
				tokens.push_back(T);
				estado=estados::leyendo_valor;
				buffer.clear();
			}
			break;
			default:
				error("Encontrado : ");
			break;
		}

	}

	void coma()
	{
		switch(estado)
		{	
			case estados::leyendo_valor:
				asignar_valor();
			break;
			case estados::subparser:
				asignar_subparser();
			break;
			default:	
				error("Encontrado , en estado incorrecto");
			break;
		}

		estado=estados::leyendo_clave;
		buffer.clear();
	}
	
	void abre_llave()
	{
		switch(estado)
		{
			case estados::leyendo_valor:
			{
//				std::cout<<"INICIANDO NUEVO PARSER..."<<std::endl;
				estado=estados::subparser;
				Parser p(fichero);
				p();
				aux=std::move(p.tokens);
			}
			break;

			default:
				error("Encontrado { en estado incorrecto : ");
			break;
		}

		buffer.clear();
	}

	void cierra_llave()
	{
		switch(estado)
		{
			case estados::leyendo_valor:
				asignar_valor();
			break;
			case estados::subparser:
				asignar_subparser();
			break;			
			default:
				error("Encontrado { en estado incorrecto");
			break;
		}

//		std::cout<<"LEO } CIERRO PARSER Y MI ESTADO ES SALIR"<<std::endl;

		estado=estados::salir;
		buffer.clear();
	}

	void abre_corchete()
	{

	}

	void cierra_corchete()
	{

	}

	void asignar_valor()
	{
		tokens.back().asignar_valor(buffer.substr(0, buffer.size()-1));
//std::cout<<"ASIGNANDO VALOR A TOKEN "<<tokens.back().valor<<std::endl;
	}

	void asignar_subparser()
	{
		tokens.back().asignar_tokens(aux);
//std::cout<<"ASIGNANDO "<<aux.size()<<" TOKENS A TOKEN"<<std::endl;
		aux.clear();
	}

	void error(const std::string& msj)
	{
		throw std::runtime_error(msj+"\nESTADO:"+traducir_estado()+"\nBUFFER:"+buffer);
	}

	std::string traducir_estado()
	{
		switch(estado)
		{
			case estados::leyendo_clave: return "LEYENDO_CLAVE"; break;
			case estados::leyendo_valor: return "LEYENDO_VALOR"; break;
			case estados::subparser: return "SUBPARSER"; break;
			case estados::salir: return "SALIR"; break;
		}
	}	

	enum class estados {leyendo_clave, leyendo_valor, subparser, salir};

	estados			estado;
	std::ifstream&		fichero;
	std::string 		buffer;
	std::vector<Token>	tokens;
	std::vector<Token>	aux;

};

void dime_token(const Token& t, int tabs=0)
{
	std::string ver_tabs;
	for(int i=0; i<tabs; ++i) ver_tabs+="\t";
	std::cout<<ver_tabs<<t.clave<<":";

	switch(t.tipo)
	{
		case Token::tipos::valor:
			std::cout<<t.valor<<std::endl;
		break;
		case Token::tipos::compuesto:
			std::cout<<ver_tabs<<"{"<<std::endl;
			for(const auto& tok : t.tokens)
				dime_token(tok, tabs+1);
			std::cout<<ver_tabs<<"}"<<std::endl;
		break;
		default:
			std::cout<<"TIPO DESCONOCIDO"<<std::endl;
		break;
	}
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
