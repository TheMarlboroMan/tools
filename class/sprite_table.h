#ifndef TOOLS_SPRITE_TABLE_H
#define TOOLS_SPRITE_TABLE_H

#include <fstream>
#include <map>
#include <vector>

#include <tools/tools/tools.h>
#include <video/rect/rect.h>

#include "text_reader.h"

namespace tools
{

//!A frame for the sprite table consisting of the size and position of the 
//!sprite plus a possible displacement pair.

struct sprite_frame {
	public:

	int 			x,	//!< X position in the sheet.
				y;	//!< Y position in the sheet.
	unsigned int 		w, 	//!< Width.
				h;	//!< Height.
	int 			disp_x, //!< Horizontal displacement.
				disp_y;	//!< Vertical displacement.

	//!Returns a libdansdl2 rect from the sprite.
	ldv::rect		get_rect() const {return ldv::rect{x, y, w, h};}

	//!Can be used to check that the sprite has been loaded. Will discard
	//!unitialized sprites.
	explicit operator bool() const {return x || y || w || h || disp_x || disp_y;}

	//!Class constructor.
	sprite_frame()
		:x(0), y(0), w(0), h(0), disp_x(0), disp_y(0) {}
};

//!Alpha and resource agnostic sprite table: contains a collection of sprite
//!frames loaded from a file with a specific format.

//!The format used uses # to comment lines and tabs to separate values.
//!# Frame id	X	Y	W	H	DESPX	DESPY
//!# Standing.
//!1	0	0	22	32	0	0

class sprite_table {
	public:

	//!Initializes the table with the file at the given path. Will throw 
	//!std::runtime error if the file cannot be found or has an invalid
	//!format.
							sprite_table(const std::string&);

	//!Loads/reloads the table with the given file path. Will throw with
	//!std::runtime_error if the file cannot be found or has an invalid
	//!format.
	void 						load(const std::string&);

	//!Returns the frame at the given index.
	const sprite_frame& 				get(size_t) const;

	//!Returns the frame at the given index.
	sprite_frame 					get(size_t);

	private:

	std::map<size_t, sprite_frame> 			data;	//!< Internal data storage.

};

}

#endif
