#include <iostream>
#include <iomanip>
#include <chrono>
#include <thread>

#include <tools/chrono.h>

int main(int, char **) {

	tools::chrono clock;
	clock.start();

	std::cout<<std::setw(2)<<std::setfill('0');

	while(true) {

		const auto full=clock.get_full();
		std::cout<<full.hours<<":"<<full.minutes<<":"<<full.seconds<<":"<<full.milliseconds<<std::endl;
		if(full.seconds >= 2) {

			break;
		}
	}

	clock.pause();
	int i=0;

	while(true) {

		const auto full=clock.get_full();
		std::cout<<i<<"/"<<5<<" "<<full.hours<<":"<<full.minutes<<":"<<full.seconds<<":"<<full.milliseconds<<std::endl;
		std::this_thread::sleep_for(std::chrono::seconds(1));

		i++;
		if(i > 4) {
			break;
		}
	}
	
	clock.resume();
	while(true) {

		const auto full=clock.get_full();
		std::cout<<full.hours<<":"<<full.minutes<<":"<<full.seconds<<":"<<full.milliseconds<<std::endl;
		if(full.seconds >= 4) {

			break;
		}
	}

	return 0;
}

