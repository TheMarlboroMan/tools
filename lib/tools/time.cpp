#include "tools/time.h"

#include <sstream>
#include <iomanip>

using namespace tools;

timedata time::seconds_to_timedata(
	unsigned long int _seconds
) const {

	int seconds=(_seconds) % 60;
	int minutes=(_seconds / 60) % 60;
	int hours=(_seconds / (60*60)) % 24;

	return {hours, minutes, seconds};
}

std::string time::time_to_string(
	int _hours,
	int _minutes,
	int _seconds
) const {

	std::stringstream ss;
	ss<<std::setfill('0')<<std::setw(2)<<_hours<<":"<<std::setw(2)<<_minutes<<":"<<std::setw(2)<<_seconds;
	return ss.str();
}
