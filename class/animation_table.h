#ifndef TOOLS_ANIMATION_TABLE_H
#define TOOLS_ANIMATION_TABLE_H

#include <map>
#include <vector>
#include <stdexcept>
#include <sstream>
#include <iterator>

#include "sprite_table.h"

namespace tools
{

//!Defines a sprite animation frame.
struct animation_line {
	public:
	//!Class constructor.
					animation_line(float d, float m, const sprite_frame& f)
						:duration(d), begin_time(m), frame(f) {}
	//!Checks the line has data.
					explicit operator bool() const {return duration && frame;}

	float 				duration=0.0f;		//!< Frame duration in ms.
	float				begin_time=0.0f;	//!< Starting ms of the frame... or maybe ending. Who knows.
	sprite_frame 			frame;			//!< Frame data.
};

class animation_table;

//!Defines an animation as a named collection of frames.
class animation {
	public:
	//!Class constructor.
					animation();

	//!Checks the animation contains data.
					explicit operator bool() const {return duration > 0.0 || data.size() > 0;}

	//!Returns the frame at the given index. Will throw if the index is not valid.
	const animation_line&		get(size_t v) const {return data.at(v);}
	
	//!Returns the frame at the given index. Will throw if the index is not valid.
	animation_line			get(size_t v) {return data.at(v);}

	//!Given a moment in time expressed as a float, returns the corresponding
	//!frame of the animation Values larger than the animation length will 
	//!be treated as if the animation loops.
	const animation_line&		get_for_time(float) const;


	//!Given a moment in time, returns the corresponding frame if the 
	//!animation would have a duration defined by the second parameter.
	const animation_line&		get_for_time(float, float) const;

	//!Returns the number of frames in the animation.
	size_t				size() const {return data.size();}

	//!Returns the animation name.
	const std::string&		get_name() const {return name;}

	//!Returns the animation duration.
	float				get_duration() const {return duration;}

	private:

	//!Calculates animation duration and adjusts starting time of each 
	//!frame... I honestly don't know why anymore.
	void				adjust_frame_time();

	std::string			name;		//!< Animation name.
	std::vector<animation_line>	data;		//!< Internal storage.
	float				duration;	//!< Calculated duration.

	friend class animation_table;

};

//!Collection of indexed animations and their associated sprite table. Designed
//!so all animations are related to each other in the application domain (so
//!they share a sprite table).
class animation_table {

	public:

	//!Class constructor. No animation data will be loaded.
					animation_table(const sprite_table&);

	//!Class constructor, loads animation data.
					animation_table(const sprite_table&, const std::string&);

	//!Loads animation data from the given filename.
	//TODO: Does not reset existing data!!!.
	void 				load(const std::string&);

	//!Returns the animation at the given index. Will throw if the index is invalid.
	const animation& 		get(size_t v) const {return data.at(v);}

	//!Returns the animation at the given index. Will throw if the index is invalid.
	animation			get(size_t v) {return data[v];}

	//!Returns the sprite table.
	const sprite_table&		get_table() const {return table;}

	//!Returns the quantity of animations in the internal storage.
	size_t				size() {return data.size();}

	private:

	//!Reads animation header from line.
	size_t				read_header(const std::string&);

	//!Reads animation data from line.
	void				read_line(const std::string&, animation&);

	const sprite_table&		table;	//!< Reference to the sprite table.
	std::map<size_t, animation>	data;	//!< Internal storage.
};

}

#endif
