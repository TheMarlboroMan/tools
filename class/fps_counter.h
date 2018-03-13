#ifndef TOOLS_FPS_COUNTER_H
#define TOOLS_FPS_COUNTER_H


#include <SDL2/SDL.h>
#include <vector>
#include <algorithm>
#include <chrono>

namespace tools
{

class fps_counter
{
	public:

	typedef		float	tdelta;
			fps_counter();

	unsigned int 	get_frame_count() const {return frame_count;}
		
	//!Must be called before drawing.
	void		begin_time_produce();
	//!Must be called when drawing is done.
	tdelta		end_time_produce();
	//!Also called after drawing is done, to count frames.
	void 		loop_step();
	//!Fills the first parameter with time until the second one is reached. A glorified "wait" or "sleep".
	void		fill_until(tdelta&, tdelta);

	private:

	typedef		std::chrono::high_resolution_clock t_clock;

	t_clock::time_point 				ticks_count,
		 					ticks_begin,
							ticks_produce;

	int 		frame_count, internal_count;
};

}
#endif
