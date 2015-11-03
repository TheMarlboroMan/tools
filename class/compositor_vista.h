#ifndef COMPOSITOR_VISTA_H
#define COMPOSITOR_VISTA_H

#include <memory>

#include "dnot_parser.h"
#include <video/pantalla/pantalla.h>
#include <video/representacion/representacion.h>
#include <video/representacion/representacion_primitiva/representacion_primitiva_caja/representacion_primitiva_caja.h>

namespace Herramientas_proyecto
{
class Compositor_vista
{
	public:

	typedef std::unique_ptr<DLibV::Representacion> uptr_representacion;


	void			parsear(const std::string&, const std::string&); //Puede tirar std::runtime_error.
	void			volcar(DLibV::Pantalla&);
	DLibV::Representacion * obtener_por_id(const std::string&);

	private:

	uptr_representacion	 crear_caja(const Dnot_token&);


	std::vector<uptr_representacion>		representaciones;
	std::map<std::string, DLibV::Representacion*>	mapa_ids;
};
}

#endif
