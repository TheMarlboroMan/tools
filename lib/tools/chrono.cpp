#include <tools/chrono.h>

using namespace tools;

void chrono::start() {

	begin=std::chrono::system_clock::now();
	running=true;
}

void chrono::stop() {

	end=std::chrono::system_clock::now();
	running=false;
}

void chrono::pause() {

	if(paused) {

		return;
	}

	paused=true;
	pause_point=std::chrono::system_clock::now();
}

void chrono::resume() {

	if(!paused) {

		return;
	}

	auto now=std::chrono::system_clock::now();
	int ms_elapsed_since_pause=std::chrono::duration_cast<std::chrono::milliseconds>(now-pause_point).count();
	begin+=std::chrono::milliseconds(ms_elapsed_since_pause);
	paused=false;
}

unsigned long int chrono::get_milliseconds() const {

	if(running) {

		if(paused) {
			return std::chrono::duration_cast<std::chrono::milliseconds>(pause_point - begin).count();
		}
		else {

			return std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now() - begin).count();
		}
	}
	else {
		return std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count();
	}
}

unsigned long int chrono::get_seconds() const {

	if(running) {

		if(paused) {
			return std::chrono::duration_cast<std::chrono::milliseconds>(pause_point - begin).count();
		}
		else {
			return std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now() - begin).count();
		}
	}
	else {
		return std::chrono::duration_cast<std::chrono::seconds>(end - begin).count();
	}
}

chrono_data chrono::get_full() {

	auto t=get_milliseconds();

	//Hyper lazy.
	auto ms=t % 1000;

	auto seconds=(t / 1000) % 60;
	auto minutes=(t / (1000*60)) % 60;
	auto hours=(t / (1000*60*60)) % 24;

	return {(unsigned)hours, (unsigned)minutes, (unsigned)seconds, (unsigned)ms};
}

void chrono::reset() {

	begin=tp{};
	end=tp{};
	pause_point=tp{};
	running=false;
	paused=false;
}
