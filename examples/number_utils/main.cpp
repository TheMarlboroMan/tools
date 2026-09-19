#include <iostream>

#include <tools/number_utils.h>

int main(int, char **) {

	std::cout<<"lerp, from -10 to 10\n";

	double start=-10, 
			end=10,
			time=0.,
			step=0.05;

	while(time <= 1.) {

		std::cout<<"t="<<time<<" "<<tools::lerp(start, end, time)<<"\n";
		time+=step;
	}

	std::cout<<"\n\nlerp, from 10 to -10\n";
	start=10;
	end=-10;
	time=0.;

	while(time <= 1.) {

		std::cout<<"t="<<time<<" "<<tools::lerp(start, end, time)<<"\n";
		time+=step;
	}
}
