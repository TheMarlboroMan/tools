#include <iostream>

#include "class/number_generator.h"
#include "templates/ranged_value.h"
#include "templates/matrix_2d.h"
#include "templates/linear_timed_function.h"
#include "templates/options_menu.h"

int main(int argc, char ** argv)
{
	tools::int_generator g(3, 6);
	std::cout<<g()<<std::endl;

	tools::ranged_value<int> rval(0, 5, -10);

	for(int i=0; i<10; ++i) 
	{
		std::cout<<rval<<std::endl;
		++rval;
	}

	tools::matrix_2d<int> thing(20,20);
	thing.insert(10,10, 33);
	thing.insert(8,8, 30);

	struct {void operator()(int v) {std::cout<<v<<std::endl;}} fuck;
	thing.apply(fuck);

	auto ltf=tools::linear_timed_function<float>::from_time(0.f, 1.f, 3.f);

	std::cout<<"==================="<<std::endl;

	while(!ltf.is_done())
	{
		std::cout<<ltf<<std::endl;
		ltf.tic(0.23f);
	}

	tools::options_menu<std::string> menu;
	std::map<std::string, int> translation;

	//TODO: I really don't understand how this component works. At all.
	//TODO: I also don't know crap about how the translation works.
	options_menu_from_dnot("options_menu.dnot", "config_app", menu, translation);
	std::cout<<menu.size()<<std::endl;

	
}
