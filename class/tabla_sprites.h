#ifndef TABLA_SPRITES_H
#define TABLA_SPRITES_H

/*La tabla frames nos permitiría abrir un archivo con un cierto formato y usarlo
para obtener los valores que nos permiten recortar una hoja de sprites en 
frames... El archivo tendría este aspecto:

#IDFRA	X	Y	W	H	DESPX	DESPY
#En pie.
1	0	0	22	32	0	0
#Caminando...
2	23	0	22	32	-1	1

Y lo usaríamos más o menos de esta forma:

Tabla_sprites TABLA_SPRITES("archivo_tabla_frames.txt");
const Frame_sprites& f=TABLA_SPRITES.obtener(0);

if(f)
{
	int x=f.x;
	int y=f.y;
	unsigned int w=f.w;
	unsigned int h=f.h;
	int desplazamiento_x=f.desp_x;
	int desplazamiento_y=f.desp_y;
}

La tabla de sprites es intencionalmente agnóstica con respecto al alpha o al
recurso que se va a usar. Se pueden tener, por ejemplo, dos recursos que
comparten la misma hoja de sprites aunque representan cosas distintas.
*/

#include <fstream>
#include <map>
#include <vector>

#include <herramientas/herramientas/herramientas.h>
#include "lector_txt.h"
#include <herramientas/log_base/log_base.h>

extern DLibH::Log_base LOG;


namespace Herramientas_proyecto
{

struct Frame_sprites
{
	public:

	int x, y;
	unsigned int w, h;
	int desp_x, desp_y;
	explicit operator bool() const {return x || y || w || h || desp_x || desp_y;}

	Frame_sprites()
		:x(0), y(0), w(0), h(0), desp_x(0), desp_y(0)
	{}
};

class Tabla_sprites
{
	public:
							Tabla_sprites(const std::string&);
							Tabla_sprites();
	void 						cargar(const std::string&);
	const Frame_sprites& 				obtener(size_t) const;
	Frame_sprites 					obtener(size_t);

	private:

	std::map<size_t, Frame_sprites> 		mapa;

};

}

#endif
