#ifndef TOOLS_FPS_COUNTER_H
#define TOOLS_FPS_COUNTER_H


#include <SDL2/SDL.h>
#include <vector>
#include <algorithm>
#include <chrono>

namespace tools
{

//!Frames-per-second counter: produces the quantity of FPS when there's drawing
//!involved.
class fps_counter {
	public:

	typedef		float	tdelta;		//!< Defines the elapsed time type (1.0 is a second).

	//!Class constructor.
			fps_counter();

	//!Returns the current frame count.
	unsigned int 	get_frame_count() const {return frame_count;}
		
	//!Starts counting. Must be called before drawing.
	void		begin_time_produce();

	//!Produces delta time elapsed, must be called when drawing is done.
	tdelta		end_time_produce();

	//!Counts frames, alse called after drawing is done.
	void 		loop_step();

	//!Fills the first parameter with time until the second one is reached. A glorified and blocking "wait" or "sleep" 
	void		fill_until(tdelta&, tdelta);

	private:

	typedef		std::chrono::high_resolution_clock t_clock;	//!< Internal clock.

	t_clock::time_point	ticks_count,	//!< Marks the beginning of a count.
				ticks_produce;	//!< Marks the beginning of a count to produce delta.

	int 			frame_count,	//!< Live frame count.
				internal_count;	//!< Mutable frame-count (will become live when a second elapses).
};

}
#endif
