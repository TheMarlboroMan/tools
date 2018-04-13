#ifndef TOOLS_STRING_UTILS_H
#define TOOLS_STRING_UTILS_H

#include <string>
#include <vector>
#include <map>

/*
Conjunto de utilidades libres con dependencias de pequeñas clases sueltas.

La idea es reemplazar en la medida de lo posible a muchas funciones que están
en libdansdl2 y que no deben estar ahí (manipulación de cadenas, porcentajes,
etc...
*/

namespace tools
{

std::vector<std::string> 		explode(const std::string &, const char, size_t max=0);
std::vector<std::string> 		explode(const std::string &, const std::string&, size_t max=0);
std::map<std::string, std::string> 	map_pair(const std::string&, const char, const char);
size_t					int_digits(int);

std::string&				replace(std::string&, const std::string&, const std::string&);
std::string& 				ltrim(std::string&);
std::string& 				rtrim(std::string&);
std::string& 				trim(std::string&);

std::string 				str_ltrim(const std::string&);
std::string 				str_rtrim(const std::string&);
std::string 				str_trim(const std::string&);
std::string				str_replace(const std::string&, const std::string&, const std::string&);

unsigned short int 			utf8_begin_bytes(const char);
bool 					is_utf8_begin_2b(const char);
bool 					is_utf8_begin_3b(const char);
bool 					is_utf8_begin_4b(const char);
bool 					is_utf8_begin_5b(const char);
bool 					is_utf8_begin_6b(const char);
unsigned char 				utf8_2b_to_uchar(char, char);
char * 					wstring_to_8b(const wchar_t *);

std::string 				split_to_lines(const std::string&, unsigned int);

std::string 				as_binary(char);
std::string 				as_binary(const std::string&);
//TODO: I don't think this belongs here.
unsigned char 				twos_complement(char);

}

#endif
