#include "tabla_sprites.h"

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
		LOG<<"ERROR: Para Tabla_sprites no se ha podido abrir el archivo "<<ruta<<std::endl;
		throw std::runtime_error("No se puede localizar archivo de frames");
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
				LOG<<"Fin fichero tabla sprites "<<ruta<<std::endl;
				break;
			}

			std::vector<std::string> valores=DLibH::Herramientas::explotar(linea, separador);
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
				LOG<<"ERROR: En tabla sprites, la línea "<<L.obtener_numero_linea()<<" no está bien formada."<<std::endl;
				throw std::runtime_error("Linea de frame mal formada");
			}
		}
	}
}

