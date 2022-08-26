#include <iostream>
#include <fstream>
#include <stdexcept>
#include <tools/lib.h>

int main(int /*argc*/, char ** /*argv*/) {

	std::cout<<"tools library version "<<tools::get_lib_version()<<std::endl;
	return 0;
}
