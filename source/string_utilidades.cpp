#include "string_utilidades.h"
#include "../class/lector_txt.h"
#include "../templates/parches_compat.h"
#include <iostream>

using namespace Herramientas_proyecto;

std::vector<std::string> Herramientas_proyecto::explotar(const std::string & p_cadena, const char p_delimitador, size_t max)
{
	size_t cuenta=1;

	std::vector<std::string> resultado;
	std::string::const_iterator ini=p_cadena.begin(), fin=p_cadena.end();
	std::string temp;

	while(ini < fin)
	{
		if( (*ini)==p_delimitador)
		{
			++cuenta;
			resultado.push_back(temp);
			temp="";
			if(max && cuenta >= max) 
			{	
				temp=std::string(ini+1, p_cadena.end());
				break;
			}
		}
		else
		{
			temp+=*ini;
		}
		
		++ini;
	}

	resultado.push_back(temp);
	return resultado;
}

/*
* @param std::string fichero: nombre del fichero a abrir.
* @param const char separador: cadena de separación entre claves y valores.
* @return std::map<std::string, std::string>
* @throw std::runtime error : cuando no se puede abrir el fichero.
*
* Abre el archivo y lee todas las líneas, explotándolas por el carácter de
* separación. De separador a izquierda se considera "clave" y de separador
* a derecha se considera "valor". Devuelve un mapa de claves y valores.
* Las líneas sin longitud y las que comienzan por "comentario" se ignoran. 
* Cuando la línea tiene más de un separador se usa sólo el primero y el 
* resto pasa a formar parte del "valor".
* En caso de claves repetidas se usará el último valor presente en el 
* archivo.
* En caso de producirse un error a la hora de leer el archivo (por ejemplo,
* el fichero no existe) se lanzará una excepción del tipo std::runtime_error.
*/

std::map<std::string, std::string> Herramientas_proyecto::generar_mapa_pares(const std::string& fichero, const char separador, const char comentario)
{
	Lector_txt L(fichero.c_str(), comentario);

	if(!L)
	{
		throw std::runtime_error("Herramientas_proyecto::generar_mapa_pares no pudo abrir el fichero "+fichero);
	}

	std::map<std::string, std::string> res;
	std::string linea;

	while(true)
	{
		linea=L.leer_linea();

		if(L.es_eof()) 
		{	
			break;
		}
		//Localizar separador... 
		else if(linea.find(separador)==linea.npos)
		{	
			throw std::runtime_error("Herramientas_proyecto::generar_mapa_pares linea malformada "+compat::to_string(L.obtener_numero_linea())+" '"+linea+"' en "+fichero);
		}

		std::vector<std::string> valores=explotar(linea, separador, 2);
		res[valores[0]]=valores[1];
	}

	return res;
}

size_t Herramientas_proyecto::digitos_en_entero(int p_entero)
{
	if(p_entero==0) return 1;

	int copia=p_entero;
	int resultado=0;

	if(copia < 0) copia=-copia;
		
	while(copia > 0)
	{
		resultado++;
		copia/=10;
	}
	return resultado;
}

void Herramientas_proyecto::reemplazar_str(std::string& sujeto, const std::string& busca, const std::string& reemplaza)
{
	size_t pos = 0, l=reemplaza.length();
	while ((pos = sujeto.find(busca, pos)) != std::string::npos) 
	{
		sujeto.replace(pos, busca.length(), reemplaza);
		pos += l;
	}
}

std::string Herramientas_proyecto::reemplazar_str(const std::string& sujeto, const std::string& busca, const std::string& reemplaza)
{
	std::string res=sujeto;
	reemplazar_str(res, busca, reemplaza);
	return res;
}

unsigned short int Herramientas_proyecto::tipo_inicio_utf8(const char c)
{
	if(es_inicio_utf8_6b(c)) return 6;
	else if(es_inicio_utf8_5b(c)) return 5;
	else if(es_inicio_utf8_4b(c)) return 4;
	else if(es_inicio_utf8_3b(c)) return 3;
	else if(es_inicio_utf8_2b(c)) return 2;
	else return 0;
}

bool Herramientas_proyecto::es_inicio_utf8_2b(const char c)
{
	return c & 128;
}

bool Herramientas_proyecto::es_inicio_utf8_3b(const char c)
{
	return es_inicio_utf8_2b(c) && (c & 64);
}

bool Herramientas_proyecto::es_inicio_utf8_4b(const char c)
{
	return es_inicio_utf8_3b(c) && (c & 32);
}

bool Herramientas_proyecto::es_inicio_utf8_5b(const char c)
{
	return es_inicio_utf8_4b(c) && (c & 16);
}

bool Herramientas_proyecto::es_inicio_utf8_6b(const char c)
{
	return es_inicio_utf8_5b(c) && (c & 8);
}

unsigned char Herramientas_proyecto::utf8_2b_a_uchar(char a, char b)
{
	a=a & 3;	//Nos quedamos con los 2 bits de la derecha.
	a=a<<6;		//Desplazamos 6 a la izquierda para que esos dos estén a la izquierda del todo.
	b=b & 63;	//Nos quedamos con los 6 bits de la derecha.

	unsigned char r=a|b;	//Combinamos.
	return r;
}

char * Herramientas_proyecto::cadena_w_a_8(const wchar_t * p_cadena)
{
	char * buffer=new char[1024];
	unsigned int i=0;

	std::locale loc;

	while(p_cadena[i]!='\0')
	{
		buffer[i]=std::use_facet<std::ctype<wchar_t> >(loc).widen(p_cadena[i]);
		i++;
	}

	buffer[i]='\0';

//			=st.narrow(p_cadena[i], '?');

	return buffer;
}

//Corta una cadena en varias líneas con el máximo de p_maximo caracteres
//por línea.
std::string Herramientas_proyecto::cadena_a_maximo_ancho(std::string const &p_cadena, unsigned int p_maximo)
{
	unsigned int leidos=0;
	std::string resultado;
	auto it=std::begin(p_cadena), fin=std::end(p_cadena);
	
	while(it != fin)
	{
		++leidos;
		resultado+=(*it);

		//TODO: REvisar: si hacemos líneas de exactamente el mismo
		//ancho hace dos veces \n.

		if((*it)=='\n') 
		{
			leidos=0;
		}
		else if(leidos % p_maximo==0)
		{
			//TODO
			//Aquí cabe un poco más de rollo... 
			//Por ejemplo, si lo que hemos leido es un 
			//espacio lo podemos quitar. Si vamos a partir
			//una cadena en dos podemos directamente cortar
			//hacia atrás (siempre que haya un espacio cerca)
			//Y pasar a la siguiente.

			resultado+='\n';
		}

		++it;
	}

	return resultado;
}
