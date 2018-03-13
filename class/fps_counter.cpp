#include "fps_counter.h"
#include <iostream>

using namespace tools;

fps_counter::fps_counter():
	ticks_count(t_clock::now()), 
	ticks_begin(ticks_count), 
	ticks_produce(ticks_count),
	frame_count(0), internal_count(0)
{

}

void fps_counter::loop_step()
{
	auto ticks_end=t_clock::now();
	++internal_count;

	//We check if a second has elapsed, to reset the framecount... Integral duration: requires duration_cast
	auto int_ms=std::chrono::duration_cast<std::chrono::milliseconds>(ticks_end - ticks_count);
	if(int_ms.count() > 1000)
	{
		//TODO: There's a slight change this would NOT be accurate... int_ms.count could be 2000, for example.
		ticks_count=t_clock::now();
		frame_count=internal_count;
		internal_count=0;
	}
}

void fps_counter::fill_until(tdelta& produced, tdelta pdelta) {

	auto start=t_clock::now();
	float diff=pdelta-produced, curr=0.f;
	while(curr < diff) {
		curr=std::chrono::duration_cast<std::chrono::milliseconds>(t_clock::now() - start).count() / 1000.f;
	}
	produced+=curr;
}

void fps_counter::begin_time_produce() {
	ticks_produce=t_clock::now();
}

fps_counter::tdelta fps_counter::end_time_produce() {
	auto int_ms=std::chrono::duration_cast<std::chrono::milliseconds>(t_clock::now() - ticks_produce);
	return int_ms.count() / 1000.0;
}
