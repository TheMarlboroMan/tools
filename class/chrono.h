#ifndef TOOLS_CHRONO_H
#define TOOLS_CHRONO_H

#include <chrono>

namespace tools
{

//!Simple chronometer.

//!Returns the time elapsed since "start" was called as an integer. 
//!There is not need to reset it: after stop, each call to start is valid.
//!In this case, time is "real" or "user" time.

struct chrono_data
{
	int	hours;
	int	minutes;
	int	seconds;
	int	milliseconds;
};

class chrono
{
	public:
	
	//!Starts the count.
	void			start();

	//!Stops the counter.
	void			stop();

	//!Returns the total of ms elapsed.
	int			get_milliseconds();

	//!Returns the total of seconds elapsed.
	int			get_seconds();

	//!Gets a full structure of hours, minutes, seconds and milliseconds.
	chrono_data		get_full();

	//!Returns true if started.
	bool			is_running() const {return running;}

	private:

	std::chrono::high_resolution_clock::time_point 	begin,
							end;
	bool						running=false;
};

}

#endif
