#ifndef STRING_UTILIDADES_H
#define STRING_UTILIDADES_H

#include <string>
#include <vector>
#include <map>

/*
Conjunto de utilidades libres.

La idea es reemplazar en la medida de lo posible a muchas funciones que están
en libdansdl2 y que no deben estar ahí (manipulación de cadenas, porcentajes,
etc...

Algunas tendrán que quedarse en libdansdl2 (por ejemplo, las que hacen falta
para separar las representaciones de texto) pero otras las podremos sacar fuera.

Ya de paso podriamos sacar fuera tambien el localizador base y el controlador
de argumentos e integrarlos con "Herramientas proyecto".

//TODO: Extraer localizador base de libdansdl2
//TODO: Reparar los cambios derivados.
//TODO: Revisar "Herramientas" en libdansdl2 porque casi todo puede irse fuera.
*/

namespace Herramientas_proyecto
{

std::vector<std::string> 		explotar(const std::string &, const char, size_t max=0);
std::map<std::string, std::string> 	generar_mapa_pares(const std::string& fichero, const char separador);
size_t					digitos_en_entero(int);


}

#endif
