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

			fps_counter();

	unsigned int 	get_frame_count() const {return frame_count;}
	
	void		reset();
	void 		end_loop_step();		//Esto lo llamamos justo después del render (o antes, lo que sea).

	//Nueva interface...
	void 		init_loop_step(float=0);
	bool 		consume_loop(float);

	private:

	std::chrono::high_resolution_clock::time_point 	ticks_count,
		 					ticks_begin;

	int 		frame_count, internal_count;

	float 		delta_acumulator,
	 		timestep;
};

}
#endif
