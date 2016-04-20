#include "gestor_fuentes_ttf.h"

#include <stdexcept>

using namespace Herramientas_proyecto;

const DLibV::Fuente_TTF& Gestor_fuentes_TTF::obtener_fuente(const std::string& f, int t) const
{
	if(!fuentes.count({f, t}))
	{
		throw std::runtime_error("La fuente "+f+" no está registrada en el tamaño solicitado");
	}

	return fuentes.at({f,t});
}

void Gestor_fuentes_TTF::registrar_fuente(const std::string& f, int t, const std::string& r)
{
	if(!fuentes.count({f, t}))
	{
		fuentes.insert( std::pair<info_fuente, DLibV::Fuente_TTF>({f, t}, DLibV::Fuente_TTF(r, t) ) );
	}
}
