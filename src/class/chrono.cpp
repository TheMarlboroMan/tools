#include "chrono.h"

using namespace tools;

void chrono::start() {
	begin=std::chrono::high_resolution_clock::now();
	running=true;
}

void chrono::stop() {
	end=std::chrono::high_resolution_clock::now();
	running=false;
}

int chrono::get_milliseconds() {
	if(running) return std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - begin).count();
	else return std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count();
}

int chrono::get_seconds() {
	if(running) return std::chrono::duration_cast<std::chrono::seconds>(std::chrono::high_resolution_clock::now() - begin).count();
	else return std::chrono::duration_cast<std::chrono::seconds>(end - begin).count();
}

chrono_data chrono::get_full() {
	auto t=get_milliseconds();

	//Hyper lazy.
	int ms=t % 1000;
	int seconds=(ms/1000) % 60;
	int minutes=((ms/1000) % 60) % 60;
	int hours=(((ms/1000) % 60) % 60) % 60;
	
	return {hours, minutes, seconds, ms};
}
