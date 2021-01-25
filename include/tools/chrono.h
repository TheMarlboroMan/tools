#pragma once

#include <chrono>

namespace tools{

//!Structure to be returned from the chrono class containing an ordered measure
//!of time.

struct chrono_data {
	unsigned    hours,		//!< Hours elapsed.
	            minutes,	//!< Minutes elapsed.
	            seconds,	//!< Seconds elapsed.
	            milliseconds;	//!< Milliseconds elapsed.
};

//!Simple chronometer.

//!Returns the time elapsed since "start" was called as an integer.
//!There is not need to reset it: after stop, each call to start is valid.
//!In this case, time is "real" or "user" time.


class chrono {

	public:

	//!Starts the count.
	void            start();

	//!Stops the counter.
	void            stop();

	//!Pauses the counter.
	void            pause();

	//!Resumes the counter.
	void            resume();

	//!Returns the total of ms elapsed.
	unsigned long int get_milliseconds();

	//!Returns the total of seconds elapsed.
	unsigned long int get_seconds();

	//!Gets a full structure of hours, minutes, seconds and milliseconds.
	chrono_data     get_full();

	//!Returns true if started.
	bool            is_running() const {return running;}

	//!Returns true if paused.
	bool            is_paused() const {return paused;}

	private:

	using tp=std::chrono::system_clock::time_point;

	tp              begin,		//!< Internal starting time point.
	                end,		//!< Internal end time point.
	                pause_point;      //!< Internal pause moment.
	bool            running{false},
	                paused{false}; //!< State flag.
};

}

