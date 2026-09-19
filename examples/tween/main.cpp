#include <tools/tweener.h>
#include <tools/tweener_interpolators.h>
#include <iostream>
#include <chrono>
#include <thread>

void run(
	tools::tween::tweener<double>&,
	tools::tween::interpolators::interpolating_function_interface&
);

int main(int , char ** ) {

	tools::tween::interpolators::linear i;
	tools::tween::tweener<double> t{.0, 1.0, 10.0, i};

	int type=0;
	bool exit=false;
	while(!exit) {

		std::cout<<"0-exit\n1-linear\n2-easeinquad\n3-easeoutquad\n"
		"4-easeinoutquad\n5-easeincubic\n6-easeoutcubic\n7-easeinoutcubic\n"
		"8-easeinsine\n9-easeoutsine\n10-easeinoutsine\n11-easeinelastic\n"
		"12-easeoutelastic\n13-easeoutbounce\n>>";

		std::cin>>type;
		if(std::cin.fail()) {

			std::cout<<"terrible option\n";
			std::cin.clear();
			std::cin.ignore(); //remove all stuff from the buffer... I swear the order of stuff with std streams is madness.
			continue;
		}

		switch(type) {

			case 0: exit=true; break;
			case 1: {
				tools::tween::interpolators::linear itp;
				run(t, itp);
			} break;
			case 2:{
				tools::tween::interpolators::ease_in_quad itp;
				run(t, itp);
			} break;
			case 3:{
				//bad shit
				tools::tween::interpolators::ease_out_quad itp;
				run(t, itp);
			}
			break;
			case 4:{
				//bad shit
				tools::tween::interpolators::ease_in_out_quad itp;
				run(t, itp);
			} break;
			case 5: {
				tools::tween::interpolators::ease_in_cubic itp;
				run(t, itp);
			} break;
			case 6: {
				tools::tween::interpolators::ease_out_cubic itp;
				run(t, itp);
			} break;
			case 7: {
				tools::tween::interpolators::ease_in_out_cubic itp;
				run(t, itp);
			} break;
			case 8:{
				tools::tween::interpolators::ease_in_sine itp;
				run(t, itp);
			} break;
			case 9:{
				tools::tween::interpolators::ease_out_sine itp;
				run(t, itp);
			} break;
			case 10:{
				tools::tween::interpolators::ease_in_out_sine itp;
				run(t, itp);
			} break;
			case 11:{
				tools::tween::interpolators::ease_in_elastic itp;
				run(t, itp);
			} break;
			case 12:{
				tools::tween::interpolators::ease_out_elastic itp;
				run(t, itp);
			} break;
			case 13:{
				tools::tween::interpolators::ease_out_bounce itp;
				run(t, itp);
			} break;
		}
	}

	return 0;
}

void run(
	tools::tween::tweener<double>& _t,
	tools::tween::interpolators::interpolating_function_interface& _i
) {

	_t.set_interpolator(_i);
	_t.reset(1., 70., 10.);

	while(!_t.is_finished()) {

		int spaces=std::floor(_t.tic(0.1));

		//erase current line...
		std::cout<<"\033[2K";
		std::cout<<"\033[2J";
		for(int i=0; i<spaces; i++) {

			std::cout<<" ";
		}

		std::cout<<"*\n";
		std::this_thread::sleep_for(std::chrono::milliseconds(10));
	}

	std::cout<<"\n\n";
}


