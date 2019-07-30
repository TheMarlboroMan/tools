#include <iostream>
#include "../../source/system.cpp"

int main(int /*argc*/, char ** /*argv*/) {

	try {

		auto ls=tools::exec("ls -la");
		auto fuck=tools::exec("flabbergaster");

	//	std::cout<<ls.code<<":"<<ls.output<<std::endl<<std::endl<<std::endl;
	//	std::cout<<fuck.code<<":"<<fuck.output<<std::endl<<std::endl<<std::endl;
	}
	catch(tools::exec_exception& e) {

		std::cout<<"ERROR: "<<e.what()<<std::endl;
	}

	return 1;
}