#pragma once

#include <string>

namespace tools {

struct timedata {
	int                 hours{0},
	                    minutes{0},
	                    seconds{0};
};

class time {

	public:

	timedata            seconds_to_timedata(unsigned long int) const;
	std::string         time_to_string(int, int, int) const;

};

}
