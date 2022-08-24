#include <iostream>
#include <string>
#include <stdexcept>
#include <tools/file_utils.h>
#include <tools/lib.h>

int main(int, char **) {

	std::cout<<"using lib version "<<tools::get_lib_version()<<std::endl;

	std::cout<<"testing for non existing file..."<<std::endl;

	if(tools::filesystem::exists("not_a_file.txt")) {
		std::cout<<"error, this should not happen."<<std::endl;
		return 1;
	}

	std::cout<<"testing for a valid file..."<<std::endl;
	if(!tools::filesystem::exists("one_line.txt")) {
		std::cout<<"error, this should not happen."<<std::endl;
		return 1;
	}

	std::cout<<"testing for reading an invalid file..."<<std::endl;
	try {
		tools::dump_file("not_a_file.txt");
		std::cout<<"error, this should not happen."<<std::endl;
		return 1;
	}
	catch(std::exception& e) {
		//Noop.
	}

	std::cout<<"testing for empty file..."<<std::endl;
	if(0!=tools::dump_file("empty_file.txt").size()) {
		std::cout<<"error, this should not happen."<<std::endl;
		return 1;
	}

	std::cout<<"testing for a single line file..."<<std::endl;
	std::string one_line=tools::dump_file("one_line.txt");
	if(one_line!="This is just a single line, ends with a period.") {
		std::cout<<"error, this should not happen"<<std::endl;
		std::cout<<"got '"<<one_line<<"'"<<std::endl;
		return 1;
	}

	std::cout<<"testing for multiple lines file..."<<std::endl;
	std::string more_lines=tools::dump_file("more_than_one_line.txt");
	if(more_lines!="This is more than one line, each line ends with a period.\nThis is the second line.\nAnd this is the third line.\nAnd the document is over. No new line at the end.") {
		std::cout<<"error, this should not happen."<<std::endl;
		return 1;
	}

	return 0;
}
