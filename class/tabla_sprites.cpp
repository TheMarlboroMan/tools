#include "tabla_sprites.h"
#include "../source/string_utilidades.h"
#include "../templates/parches_compat.h"

using namespace Herramientas_proyecto;

Tabla_sprites::Tabla_sprites(const std::string& ruta)
{
	cargar(ruta);
}

Tabla_sprites::Tabla_sprites()
{

}

const Frame_sprites& Tabla_sprites::obtener(size_t indice) const
{
	return mapa.at(indice);
}

Frame_sprites Tabla_sprites::obtener(size_t indice)
{
	if(mapa.count(indice)) return mapa[indice];
	else return Frame_sprites();
}

void Tabla_sprites::cargar(const std::string& ruta)
{
	Lector_txt L(ruta, '#');

	if(!L)	
	{
		throw std::runtime_error(std::string("No se puede localizar archivo de frames:")+ruta);
	}
	else
	{
		std::string linea;
		const char separador='\t';
		
		while(true)
		{
			linea=L.leer_linea();
			if(L.es_eof()) 
			{
				break;
			}

			std::vector<std::string> valores=explotar(linea, separador);
			if(valores.size()==7)
			{
				Frame_sprites f;
				size_t indice=std::atoi(valores[0].c_str());
				f.x=std::atoi(valores[1].c_str());
				f.y=std::atoi(valores[2].c_str());
				f.w=std::atoi(valores[3].c_str());
				f.h=std::atoi(valores[4].c_str());
				f.desp_x=std::atoi(valores[5].c_str());
				f.desp_y=std::atoi(valores[6].c_str());
				mapa[indice]=f;
			}
			else
			{
				throw std::runtime_error(std::string("Linea de frame mal formada ")+ruta+std::string(" : ")+compat::to_string(L.obtener_numero_linea()));
			}
		}
	}
}

