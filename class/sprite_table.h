#ifndef TOOLS_SPRITE_TABLE_H
#define TOOLS_SPRITE_TABLE_H

/*La tabla frames nos permitiría abrir un archivo con un cierto formato y usarlo
para get los valores que nos permiten recortar una hoja de sprites en 
frames... El archivo tendría este aspecto:

#IDFRA	X	Y	W	H	DESPX	DESPY
#En pie.
1	0	0	22	32	0	0
#Caminando...
2	23	0	22	32	-1	1

Y lo usaríamos más o menos de esta forma:

sprite_table TABLA_SPRITES("archivo_tabla_frames.txt");
const sprite_frame& f=TABLA_SPRITES.get(0);

if(f)
{
	int x=f.x;
	int y=f.y;
	unsigned int w=f.w;
	unsigned int h=f.h;
	int desplazamiento_x=f.disp_x;
	int desplazamiento_y=f.disp_y;
}

La tabla de sprites es intencionalmente agnóstica con respecto al alpha o al
recurso que se va a usar. Se pueden tener, por ejemplo, dos recursos que
comparten la misma hoja de sprites aunque representan cosas distintas.
*/

#include <fstream>
#include <map>
#include <vector>

#include <tools/tools/tools.h>
#include <video/rect/rect.h>

#include "text_reader.h"

namespace tools
{

//Be intelligent: use power of two textures, please.

struct sprite_frame
{
	public:

	int 			x, y;
	unsigned int 		w, h;
	int 			disp_x, disp_y;
	ldv::rect		get_rect() const {return ldv::rect{x, y, w, h};}

	explicit operator bool() const {return x || y || w || h || disp_x || disp_y;}

	sprite_frame()
		:x(0), y(0), w(0), h(0), disp_x(0), disp_y(0)
	{}
};

class sprite_table
{
	public:
							sprite_table(const std::string&);
							sprite_table();
	void 						load(const std::string&);
	const sprite_frame& 				get(size_t) const;
	sprite_frame 					get(size_t);

	private:

	std::map<size_t, sprite_frame> 			data;

};

}

#endif
