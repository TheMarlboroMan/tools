#ifndef STRING_UTILIDADES_H
#define STRING_UTILIDADES_H

#include <string>
#include <vector>
#include <map>

/*
Conjunto de utilidades libres con dependencias de pequeñas clases sueltas.

La idea es reemplazar en la medida de lo posible a muchas funciones que están
en libdansdl2 y que no deben estar ahí (manipulación de cadenas, porcentajes,
etc...
*/

namespace Herramientas_proyecto
{

std::vector<std::string> 		explotar(const std::string &, const char, size_t max=0);
std::map<std::string, std::string> 	generar_mapa_pares(const std::string& fichero, const char separador, const char comentario);
size_t					digitos_en_entero(int);
void					reemplazar_str(std::string& sujeto, const std::string& busca, const std::string& reemplaza);
std::string				reemplazar_str(const std::string& sujeto, const std::string& busca, const std::string& reemplaza);
unsigned short int tipo_inicio_utf8(const char c);
bool es_inicio_utf8_2b(const char c);
bool es_inicio_utf8_3b(const char c);
bool es_inicio_utf8_4b(const char c);
bool es_inicio_utf8_5b(const char c);
bool es_inicio_utf8_6b(const char c);
unsigned char utf8_2b_a_uchar(char a, char b);
char * cadena_w_a_8(const wchar_t * p_cadena);
std::string cadena_a_maximo_ancho(std::string const &, unsigned int);

}

#endif
