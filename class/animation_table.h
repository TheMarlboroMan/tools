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

struct animation_line
{
	public:
					animation_line(float d, float m, const sprite_frame& f)
						:duration(d), begin_time(m), frame(f) {}
					explicit operator bool() const {return duration && frame;}
	float 				duration=0.0f;				//Cuánto dura.
	float				begin_time=0.0f;			//Cuál es el último momento de su duración dentro de toda la animación.
	sprite_frame 			frame;
};

class animation_table;

class animation
{
					//No necesita constructor: el float se inicializa a 0.0f...
	public:
					animation();
					explicit operator bool() const {return duration > 0.0 || data.size() > 0;}
	const animation_line&		get(size_t v) const {return data.at(v);}
	animation_line			get(size_t v) {return data.at(v);}
	const animation_line&		get_for_time(float) const;
	const animation_line&		get_for_time(float, float) const;
	size_t				size() const {return data.size();}
	const std::string&		get_name() const {return name;}
	float				get_duration() const {return duration;}

	private:

	void				adjust_frame_time();

	std::string			name;
	std::vector<animation_line>	data;
	float				duration;

	friend class animation_table;

};

class animation_table
{
	public:
					animation_table(const sprite_table&);
					animation_table(const sprite_table&, const std::string&);
	void 				load(const std::string&);
	const animation& 		get(size_t v) const {return data.at(v);}
	animation			get(size_t v) {return data[v];}
	const sprite_table&		get_table() const {return table;}
	size_t				size() {return data.size();}

	private:

	size_t				read_header(const std::string&);
	void				read_line(const std::string&, animation&);
	const sprite_table&		table;
	std::map<size_t, animation>	data;
};

}

#endif
