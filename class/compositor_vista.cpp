#include "compositor_vista.h"
using namespace Herramientas_proyecto;

void Compositor_vista::volcar(DLibV::Pantalla& p)
{
	for(const auto& r : representaciones)
	{
		r->volcar(p);
	}
}

DLibV::Representacion * Compositor_vista::obtener_por_id(const std::string& id)
{
	if(mapa_ids[id]) return mapa_ids[id];
	else return nullptr;
}

/**
* @param std::string ruta: ruta del fichero dnot a parsear.
* @param std::string nodo: nombre del nodo dnot que contiene la escena.
*
* Lanzará excepción si no es posible parsear el fichero o si el nodo no existe.
*/

void Compositor_vista::parsear(const std::string& ruta, const std::string& nodo)
{
	Dnot_token tok=parsear_dnot(ruta);
	const auto& lista=tok[nodo].acc_lista();
	for(const auto& token : lista)
	{
		uptr_representacion ptr;
		const auto& tipo=token["tipo"].acc_string(); //Si no hay tipo vamos a explotar. Correcto.

		if(tipo=="caja") ptr=std::move(crear_caja(token));
		else
		{
			throw std::runtime_error("Tipo '"+tipo+"' desconocido al parsear escena");
		}

		//Tratamiento de cosas comunes...

		if(token.existe_clave("alpha"))
		{
			ptr->establecer_alpha(token["alpha"].acc_int());
		}

		if(token.existe_clave("id"))
		{
			const std::string& id=token["id"];
			if(mapa_ids.count(id))
			{
				throw std::runtime_error("La clave '"+token["id"].acc_string()+"' ya existe para la escena");
			}
			else
			{
				mapa_ids[id]=ptr.get();
			}
		}

		//Y finalmente insertamos.
		representaciones.push_back(std::move(ptr));
	}	
}

Compositor_vista::uptr_representacion Compositor_vista::crear_caja(const Dnot_token& token)
{
	

	const auto& pos=token["pos"];
	const auto& color=token["color"];

	int x=pos[0], y=pos[1], w=pos[2], h=pos[3],
		r=color[0], g=color[1], b=color[2];

	uptr_representacion res(new DLibV::Representacion_primitiva_caja(SDL_Rect{x, y, w, h}, r, g, b));
	return res;
}
