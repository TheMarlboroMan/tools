#include "string_utilidades.h"
#include "../class/lector_txt.h"
#include <iostream>

#ifdef WINCOMPIL
/* Localización del parche mingw32... Esto debería estar en otro lado, supongo. */
#include <herramientas/herramientas/herramientas.h>
#endif

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

#ifdef WINCOMPIL
	using namespace parche_mingw;
#else
	using namespace std;
#endif

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
			throw std::runtime_error("Herramientas_proyecto::generar_mapa_pares linea malformada "+to_string(L.obtener_numero_linea())+" '"+linea+"' en "+fichero);
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
