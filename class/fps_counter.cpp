#include "fps_counter.h"
#include <iostream>

//#include "../log/log.h"

using namespace tools;

fps_counter::fps_counter():
	ticks_count(std::chrono::high_resolution_clock::now()), 
	ticks_begin(ticks_count), 
	frame_count(0), internal_count(0),
	delta_acumulator(0.f), timestep(0.f)
{

}

void fps_counter::reset()
{
	ticks_count=std::chrono::high_resolution_clock::now();
	ticks_begin=ticks_count;
}

/**
This method must be invoked right before the drawing process starts.

It setups the internal of the class to start measuring the drawing time by 
basically setting the time to consume now to the time it took the previous
render.

Timestep cap can be set by the caller to prevent very large timesteps from
happening (if, for example, the previous render took one second to complete)
By default is 0.f.
*/

void fps_counter::init_loop_step(float timestep_cap)
{
	timestep=delta_acumulator;
	if(timestep_cap && timestep > timestep_cap) timestep=timestep_cap;
	delta_acumulator=0.0f;
}

/** 
This method must be invoked in a loop to process the aplication logic in
discrete chunks (pdelta). Time consumed comes from the previous render time.
*/

bool fps_counter::consume_loop(float pdelta)
{
	//Still in the loop, we keep substracting...
	if(timestep >= pdelta)
	{
		timestep-=pdelta;
		return true;
	}
	//Leaving the loop, we save the reminder.
	else
	{
		delta_acumulator+=timestep;
		return false;
	}
}

/**
This method must be run after the drawing process is done. 

It measures the time elapsed since we began the steps, accumulating frames
once a second has passed.
*/

void fps_counter::end_loop_step()
{
	auto ticks_end=std::chrono::high_resolution_clock::now();
	++internal_count;

	//We check if a second has elapsed, to reset the framecount... Integral duration: requires duration_cast
	auto int_ms=std::chrono::duration_cast<std::chrono::milliseconds>(ticks_end - ticks_count);
	if(int_ms.count() > 1000)
	{
		//TODO: There's a slight change this would NOT be accurate... int_ms.count could be 2000, for example.
		ticks_count=std::chrono::high_resolution_clock::now();
		frame_count=internal_count;
		internal_count=0;
	}

	auto diff=std::chrono::duration_cast<std::chrono::milliseconds>(ticks_end - ticks_begin);
	delta_acumulator+=diff.count() / 1000.f;
	ticks_begin=std::chrono::high_resolution_clock::now();
}
