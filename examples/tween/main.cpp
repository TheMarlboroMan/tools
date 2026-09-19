#include <tools/tweener.h>
#include <tools/tweener_interpolators.h>
#include <iostream>
#include <iomanip>

int main(int argc, char ** argv) {

	tools::tween::interpolators::linear i;
	tools::tween::tweener<double> t{.0, 1.0, 10.0, i};

	while(!t.is_finished()) {

		auto val=t.tic(0.1);
		std::cout<<"val="<<val<<" "<<std::setprecision(16)<<t.get_current_time()<<"/10.0 finished="<<t.is_finished()<<"\n";
	}

	t.reset(0, 0 ,0);

	return 0;
}
