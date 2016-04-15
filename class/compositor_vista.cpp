#include "compositor_vista.h"
using namespace Herramientas_proyecto;

const std::string Compositor_vista::clave_tipo="tipo";
const std::string Compositor_vista::clave_caja="caja";
const std::string Compositor_vista::clave_bitmap="bitmap";
const std::string Compositor_vista::clave_ttf="ttf";
const std::string Compositor_vista::clave_patron="patron";
const std::string Compositor_vista::clave_pantalla="pantalla";
const std::string Compositor_vista::clave_constante="const";
const std::string Compositor_vista::clave_alpha="alpha";
const std::string Compositor_vista::clave_orden="orden";
const std::string Compositor_vista::clave_id="id";
const std::string Compositor_vista::clave_rgb="rgb";
const std::string Compositor_vista::clave_rgba="rgba";
const std::string Compositor_vista::clave_pos="pos";
const std::string Compositor_vista::clave_rec="rec";
const std::string Compositor_vista::clave_texto="texto";
const std::string Compositor_vista::clave_superficie="superficie";
const std::string Compositor_vista::clave_fuente="fuente";
const std::string Compositor_vista::clave_textura="textura";
const std::string Compositor_vista::clave_estatica="estatica";
const std::string Compositor_vista::clave_pincel="pincel";
const std::string Compositor_vista::clave_visibilidad="visible";
const std::string Compositor_vista::clave_externa="externa";
const std::string Compositor_vista::clave_ref_externa="ref";
const std::string Compositor_vista::clave_rotacion="rotacion";

Compositor_vista::Compositor_vista()
	:con_pantalla(false), color_pantalla{0,0,0, 255}
{

}

void Compositor_vista::volcar(DLibV::Pantalla& p)
{
	if(con_pantalla)
	{
		p.limpiar(color_pantalla.r, color_pantalla.g, color_pantalla.b, color_pantalla.a);
	}
	
	for(auto& r : representaciones)
	{
		r.volcar(p);
	}
}

void Compositor_vista::volcar(DLibV::Pantalla& p, const DLibV::Camara& cam)
{
	if(con_pantalla)
	{
		p.limpiar(color_pantalla.r, color_pantalla.g, color_pantalla.b, color_pantalla.a);
	}
	
	for(auto& r : representaciones)
	{
		r.volcar(p, cam);
	}
}

/**
* Recibe una textura y la mapea a una clave para poder usarla en diversas 
* representaciones. No hace copia de la textura: en su lugar se requiere que la
* textura exista mientras exista el compositor de vistas.
*/

void Compositor_vista::mapear_textura(const std::string& clave, DLibV::Textura * tex)
{
	if(mapa_texturas.count(clave))
	{
		throw std::runtime_error("La clave "+clave+" ya está ocupada para textura.");
	}
	
	mapa_texturas[clave]=tex;
}

void Compositor_vista::mapear_textura(const std::string& clave, DLibV::Textura& tex)
{
	mapear_textura(clave, &tex);
}

/**
* Recibe una superficie y la mapea a una clave para poder usarla en diversas 
* representaciones. No hace copia de la superficie: en su lugar se requiere que 
* exista mientras exista el compositor de vistas.
*/

void Compositor_vista::mapear_superficie(const std::string& clave, DLibV::Superficie * sup)
{
	if(mapa_superficies.count(clave))
	{
		throw std::runtime_error("La clave "+clave+" ya está ocupada para superficies.");
	}
	
	mapa_superficies[clave]=sup;
}

void Compositor_vista::mapear_superficie(const std::string& clave, DLibV::Superficie& sup)
{
	mapear_superficie(clave, &sup);
}

void Compositor_vista::mapear_fuente(const std::string& clave, const DLibV::Fuente_TTF * fuente)
{
	if(mapa_fuentes.count(clave))
	{
		throw std::runtime_error("La clave "+clave+" ya está ocupada para fuentes.");
	}
	
	mapa_fuentes[clave]=fuente;
}

void Compositor_vista::mapear_fuente(const std::string& clave, const DLibV::Fuente_TTF& fuente)
{
	mapear_fuente(clave, &fuente);
}

DLibV::Representacion * Compositor_vista::obtener_por_id(const std::string& id)
{
	try
	{
		return mapa_ids.at(id);
	}
	catch(std::exception& e)
	{
		throw std::runtime_error("La clave "+id+" no existe en la vista. ¿Está la vista montada?");
	}
}

bool Compositor_vista::existe_id(const std::string& id) const
{
	return mapa_ids.count(id);
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
		const auto& tipo=token[clave_tipo].acc_string(); //Si no hay tipo vamos a explotar. Correcto.
		int orden=0;

		if(tipo==clave_caja) ptr=std::move(crear_caja(token));
		else if(tipo==clave_bitmap) ptr=std::move(crear_bitmap(token));
		else if(tipo==clave_texto) ptr=std::move(crear_texto(token));
		else if(tipo==clave_ttf) ptr=std::move(crear_ttf(token));
		else if(tipo==clave_patron) ptr=std::move(crear_patron(token));
		else if(tipo==clave_externa)
		{
			const std::string& ref=token[clave_ref_externa];
			if(!mapa_externas.count(ref))
			{
				throw std::runtime_error("La clave '"+ref+"' no se ha registrado externamente.");
			}

			if(token.existe_clave(clave_orden)) 
			{
				orden=token[clave_orden];
			}

			representaciones.push_back(item(mapa_externas[ref], orden));
			continue;
		}
		else if(tipo==clave_pantalla) 
		{
			procesar_tipo_pantalla(token);
			continue;
		}
		else if(tipo==clave_constante)
		{
			procesar_tipo_constante(token);
			continue;
		}
		else
		{
			throw std::runtime_error("Tipo '"+tipo+"' desconocido al parsear escena");
		}

		//Tratamiento de cosas comunes...
		if(token.existe_clave(clave_orden)) 
		{
			orden=token[clave_orden];
		}

		//Tratamiento de cosas comunes...
		if(token.existe_clave(clave_alpha))
		{
			ptr->establecer_modo_blend(DLibV::Representacion::BLEND_ALPHA);
			ptr->establecer_alpha((Uint8)token[clave_alpha].acc_int());
		}

		if(token.existe_clave(clave_estatica))
		{
			ptr->hacer_estatica();
		}

		if(token.existe_clave(clave_rotacion))
		{
			if(tipo==clave_caja)
			{
				throw std::runtime_error("La rotación sólo es aplicable a representaciones gráficas");
			}
			else
			{
				auto valores=token[clave_rotacion].acc_lista();
				if(valores.size()!=3) throw std::runtime_error("La rotación recibe tres parámetros exactamente");

				auto * rrot=static_cast<DLibV::Representacion_grafica *>(ptr.get());
				rrot->transformar_rotar((int)valores[0]);
				rrot->transformar_centro_rotacion((int)valores[1], (int)valores[2]);
			}
		}

		if(token.existe_clave(clave_visibilidad))
		{
			ptr->cambiar_visibilidad(token[clave_visibilidad].acc_bool());
		}

		if(token.existe_clave(clave_id))
		{
			const std::string& id=token[clave_id];
			if(mapa_ids.count(id))
			{
				throw std::runtime_error("La clave '"+token[clave_id].acc_string()+"' ya existe para la escena");
			}
			else
			{
				mapa_ids[id]=ptr.get();
			}
		}

		//Y finalmente insertamos.
		representaciones.push_back(item(std::move(ptr), orden));
	}

	std::sort(std::begin(representaciones), std::end(representaciones));
}

void Compositor_vista::registrar_externa(const std::string& clave, DLibV::Representacion& rep)
{
	if(mapa_externas.count(clave))
	{
		throw std::runtime_error("La clave "+clave+" ya existe al registrar representación externa");
	}
	
	mapa_externas[clave]=&rep;
} 

Compositor_vista::uptr_representacion Compositor_vista::crear_caja(const Dnot_token& token)
{
	auto color=color_desde_lista(token[clave_rgb]);
	uptr_representacion res(new DLibV::Representacion_primitiva_caja(caja_desde_lista(token[clave_pos]), color.r, color.g, color.b));
	return res;
}

Compositor_vista::uptr_representacion Compositor_vista::crear_bitmap(const Dnot_token& token)
{
	if(!mapa_texturas.count(token[clave_textura]))
	{
		throw std::runtime_error("No se localiza la textura "+token[clave_textura].acc_string()+" para bitmap");
	}
	
	uptr_representacion res(new DLibV::Representacion_bitmap(mapa_texturas[token[clave_textura]]));
	res->establecer_posicion(caja_desde_lista(token[clave_pos]));
	res->establecer_recorte(caja_desde_lista(token[clave_rec]));

	return res;
}

Compositor_vista::uptr_representacion Compositor_vista::crear_texto(const Dnot_token& token)
{
	if(!mapa_superficies.count(token[clave_superficie]))
	{
		throw std::runtime_error("No se localiza la superficie "+token[clave_superficie].acc_string()+" para texto");
	}

	uptr_representacion res(new DLibV::Representacion_texto_auto(mapa_superficies[token[clave_superficie]], token[clave_texto].acc_string()));

	auto posicion=posicion_desde_lista(token[clave_pos]);
	res->establecer_posicion(posicion.x, posicion.y);

	return res;
}

Compositor_vista::uptr_representacion Compositor_vista::crear_ttf(const Dnot_token& token)
{
	if(!mapa_fuentes.count(token[clave_fuente]))
	{
		throw std::runtime_error("No se localiza la fuente "+token[clave_fuente].acc_string()+" para TTF");
	}

	uptr_representacion res(new DLibV::Representacion_TTF(*mapa_fuentes[token[clave_fuente]], rgba_desde_lista(token[clave_rgba]), token[clave_texto].acc_string()));

	auto posicion=posicion_desde_lista(token[clave_pos]);
	res->establecer_posicion(posicion.x, posicion.y);
	return res;
}

Compositor_vista::uptr_representacion Compositor_vista::crear_patron(const Dnot_token& token)
{

	if(!mapa_texturas.count(token[clave_textura]))
	{
		throw std::runtime_error("No se localiza la textura "+token[clave_textura].acc_string()+" para patrón");
	}
	
	auto res=new DLibV::Representacion_bitmap_patron(mapa_texturas[token[clave_textura]]);
	res->establecer_posicion(caja_desde_lista(token[clave_pos]));
	res->establecer_recorte(caja_desde_lista(token[clave_rec]));
	res->establecer_pincel(caja_desde_lista(token[clave_pincel]));

	return uptr_representacion(res);
}

void Compositor_vista::procesar_tipo_pantalla(const Dnot_token& token)
{
	color_pantalla=rgba_desde_lista(token[clave_rgba]);
	con_pantalla=true;
}

void Compositor_vista::procesar_tipo_constante(const Dnot_token& token)
{
	const std::string& clave=token["clave"].acc_string();

	if(constantes_int.count(clave)) throw std::runtime_error("Constante "+clave+" declarada en más de una ocasión");

	if(token["valor"].es_valor_int()) constantes_int[clave]=token["valor"].acc_int();
	else if(token["valor"].es_valor_float()) constantes_float[clave]=token["valor"].acc_float();
	else throw std::runtime_error("La clave valor no es tipo constante válido.");	
}

SDL_Rect Compositor_vista::caja_desde_lista(const Dnot_token& tok)
{
	int x=tok[0], y=tok[1], w=tok[2], h=tok[3];
	return SDL_Rect{x, y, w, h};
}

Compositor_vista::color Compositor_vista::color_desde_lista(const Dnot_token& tok)
{
	int r=tok[0], g=tok[1], b=tok[2];
	return color{r, g, b};
}

SDL_Color Compositor_vista::rgba_desde_lista(const Dnot_token& tok)
{
	int r=tok[0], g=tok[1], b=tok[2], a=tok[3];
	return SDL_Color{(Uint8)r, (Uint8)g, (Uint8)b, (Uint8)a};
}

Compositor_vista::posicion Compositor_vista::posicion_desde_lista(const Dnot_token& tok)
{
	int x=tok[0], y=tok[1];
	return posicion{x, y};
}

void Compositor_vista::vaciar_vista()
{
	representaciones.clear();
	mapa_ids.clear();
	mapa_externas.clear();
}

void Compositor_vista::vaciar_constantes()
{
	constantes_int.clear();
}

int Compositor_vista::const_int(const std::string& k) const
{
	return obtener_const(k, constantes_int);
}

float Compositor_vista::const_float(const std::string& k) const
{
	return obtener_const(k, constantes_float);
}
