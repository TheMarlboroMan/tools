#ifndef ESTRUCTURA_PAGINACION_H
#define ESTRUCTURA_PAGINACION_H

/**
* Estructura de paginación que incluye un índice para seleccionar algo.
*/

#include <cstddef>
#include <cmath>

namespace Herramientas_proyecto
{

class Estructura_paginacion
{
	public:
							Estructura_paginacion(size_t rpp=0, size_t tot=0);
	bool						cambiar_pagina(int, bool=true);
	bool						cambiar_item(int);

	void						reiniciar_indice() {indice_actual=0;}
	void						mut_indice(size_t v) {indice_actual=v;}
	size_t						acc_pagina_actual() const {return pagina_actual;}
	size_t						acc_indice_actual() const {return indice_actual;}
	size_t						acc_total_paginas() const {return total_paginas;}
	size_t						acc_total_elementos() const {return total_elementos;}
	size_t						acc_registros_por_pagina() const {return registros_por_pagina;}

	void						establecer_total_elementos(size_t);
	void						establecer_registros_por_pagina(size_t);

	private:

	void						calcular_informacion_paginas();

	size_t						registros_por_pagina;
	size_t						total_elementos;
	size_t						pagina_actual;
	size_t						total_paginas;
	size_t						indice_actual;
};

}
#endif
