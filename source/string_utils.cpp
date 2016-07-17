#include "string_utils.h"

#include <iostream>
#include <algorithm>

#include "../class/text_reader.h"
#include "../templates/compatibility_patches.h"


using namespace tools;

std::vector<std::string> tools::explode(const std::string & pstring, const char pdelimiter, size_t max)
{
	size_t count=1;

	std::vector<std::string> result;
	std::string::const_iterator ini=pstring.begin(), fin=pstring.end();
	std::string temp;

	while(ini < fin)
	{
		if( (*ini)==pdelimiter)
		{
			++count;
			result.push_back(temp);
			temp="";
			if(max && count >= max) 
			{	
				temp=std::string(ini+1, pstring.end());
				break;
			}
		}
		else
		{
			temp+=*ini;
		}
		
		++ini;
	}

	result.push_back(temp);
	return result;
}

/*
* @param std::string file: nombre del file a abrir.
* @param const char delimiter: cadena de separación entre claves y values.
* @return std::map<std::string, std::string>
* @throw std::runtime error : cuando no se puede abrir el file.
*
* Abre el archivo y lee todas las líneas, explotándolas por el carácter de
* separación. De delimiter a izquierda se considera "clave" y de delimiter
* a derecha se considera "valor". Devuelve un mapa de claves y values.
* Las líneas sin longitud y las que comienzan por "comment" se ignoran. 
* Cuando la línea tiene más de un delimiter se usa sólo el primero y el 
* resto pasa a formar parte del "valor".
* En caso de claves repetidas se usará el último valor presente en el 
* archivo.
* En caso de producirse un error a la hora de leer el archivo (por ejemplo,
* el file no existe) se lanzará una excepción del tipo std::runtime_error.
*/

std::map<std::string, std::string> tools::map_pair(const std::string& file, const char delimiter, const char comment)
{
	text_reader L(file.c_str(), comment);

	if(!L)
	{
		throw std::runtime_error("tools::map_pair could not open "+file);
	}

	std::map<std::string, std::string> res;
	std::string line;

	while(true)
	{
		line=L.read_line();

		if(L.is_eof()) 
		{	
			break;
		}
		//Localizar delimiter... 
		else if(line.find(delimiter)==line.npos)
		{	
			throw std::runtime_error("tools::map_pair malformed line "+compat::to_string(L.get_line_number())+" '"+line+"' en "+file);
		}

		std::vector<std::string> values=explode(line, delimiter, 2);
		res[values[0]]=values[1];
	}

	return res;
}

size_t tools::int_digits(int pint)
{
	if(pint==0) return 1;

	int copy=pint;
	int result=0;

	if(copy < 0) copy=-copy;
		
	while(copy > 0)
	{
		result++;
		copy/=10;
	}
	return result;
}

std::string& tools::ltrim(std::string &s) 
{
	s.erase(s.begin(), std::find_if(s.begin(), s.end(), std::not1(std::ptr_fun<int, int>(std::isspace))));	
	return s;
}

std::string& tools::rtrim(std::string &s) 
{
	s.erase(std::find_if(s.rbegin(), s.rend(), std::not1(std::ptr_fun<int, int>(std::isspace))).base(), s.end());
	return s;
}

std::string& tools::trim(std::string &s) 
{
	return ltrim(rtrim(s));
}

std::string& tools::replace(std::string& psubject, const std::string& psearch, const std::string& preplace)
{
	size_t pos = 0, l=preplace.length();
	while ((pos = psubject.find(psearch, pos)) != std::string::npos) 
	{
		psubject.replace(pos, psearch.length(), preplace);
		pos += l;
	}

	return psubject;
}

std::string tools::str_ltrim(const std::string &sub) 
{
	std::string s(sub);
	return ltrim(s);
}

std::string tools::str_rtrim(const std::string &sub) 
{
	std::string s(sub);
	return rtrim(s);
}

std::string tools::str_trim(const std::string &sub) 
{
	std::string s(sub);
	return trim(s);
}

std::string tools::str_replace(const std::string& psubject, const std::string& psearch, const std::string& preplace)
{
	std::string s(psubject);
	return replace(s, psearch, preplace);
}

unsigned short int tools::utf8_begin_bytes(const char c)
{
	if(is_utf8_begin_6b(c)) return 6;
	else if(is_utf8_begin_5b(c)) return 5;
	else if(is_utf8_begin_4b(c)) return 4;
	else if(is_utf8_begin_3b(c)) return 3;
	else if(is_utf8_begin_2b(c)) return 2;
	else return 0;
}

bool tools::is_utf8_begin_2b(const char c)
{
	return c & 128;
}

bool tools::is_utf8_begin_3b(const char c)
{
	return is_utf8_begin_2b(c) && (c & 64);
}

bool tools::is_utf8_begin_4b(const char c)
{
	return is_utf8_begin_3b(c) && (c & 32);
}

bool tools::is_utf8_begin_5b(const char c)
{
	return is_utf8_begin_4b(c) && (c & 16);
}

bool tools::is_utf8_begin_6b(const char c)
{
	return is_utf8_begin_5b(c) && (c & 8);
}

unsigned char tools::utf8_2b_to_uchar(char a, char b)
{
	a=a & 3;	//Nos quedamos con los 2 bits de la derecha.
	a=a<<6;		//Desplazamos 6 a la izquierda para que esos dos estén a la izquierda del todo.
	b=b & 63;	//Nos quedamos con los 6 bits de la derecha.

	unsigned char r=a|b;	//Combinamos.
	return r;
}

char * tools::wstring_to_8b(const wchar_t * pstring)
{
	char * buffer=new char[1024];
	unsigned int i=0;

	std::locale loc;

	while(pstring[i]!='\0')
	{
		buffer[i]=std::use_facet<std::ctype<wchar_t> >(loc).widen(pstring[i]);
		i++;
	}

	buffer[i]='\0';

//			=st.narrow(pstring[i], '?');

	return buffer;
}

//Corta una cadena en varias líneas con el máximo de pmax caracteres
//por línea.
std::string tools::split_to_lines(std::string const &pstring, unsigned int pmax)
{
	unsigned int read=0;
	std::string result;
	auto it=std::begin(pstring), fin=std::end(pstring);
	
	while(it != fin)
	{
		++read;
		result+=(*it);

		//TODO: REvisar: si hacemos líneas de exactamente el mismo
		//ancho hace dos veces \n.

		if((*it)=='\n') 
		{
			read=0;
		}
		else if(read % pmax==0)
		{
			//TODO
			//Aquí cabe un poco más de rollo... 
			//Por ejemplo, si lo que hemos leido es un 
			//espacio lo podemos quitar. Si vamos a partir
			//una cadena en dos podemos directamente cortar
			//hacia atrás (siempre que haya un espacio cerca)
			//Y pasar a la siguiente.

			result+='\n';
		}

		++it;
	}

	return result;
}