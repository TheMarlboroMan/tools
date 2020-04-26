#include <iostream>
#include <tools/options_menu.h>
#include <tools/json.h>
#include <tools/file_utils.h>

#include <rapidjson/document.h>

#include <vector>
#include <string>
#include <map>

template<typename T>
void show_menu(const tools::options_menu<T>& _menu) {

	for(const auto& key : _menu.get_keys()) {

		std::cout<<key<<" : "<<_menu.get_str_value(key)<<std::endl;
	}
}

template<typename T>
bool test_assignment_failures(tools::options_menu<T>& _menu) {

	try {
		_menu.set("22_NAME", 22);
		std::cout<<"22_NAME failed"<<std::endl;
		return false;
	}
	catch(std::exception& e) {}

	try {
		_menu.set("25_FILESIZE", "hello");
		std::cout<<"25_FILESIZE failed"<<std::endl;
		return false;
	}
	catch(std::exception& e) {}

	try {
		_menu.set("27_BACKUP", "how are you");
		std::cout<<"27_BACKUP failed"<<std::endl;
		return false;
	}
	catch(std::exception& e) {}

	try {
		_menu.set("20_HELP", "in this fine day");
		std::cout<<"20_HELP failed"<<std::endl;
		return false;
	}
	catch(std::exception& e) {}

	try {
		_menu.set("40_PERIODICITY", "sir?");
		std::cout<<"40_PERIODICITY failed"<<std::endl;
		return false;
	}
	catch(std::exception& e) {}

	try {
		_menu.set("10_WINDOW", false);
		std::cout<<"10_WINDOW failed"<<std::endl;
		return false;
	}
	catch(std::exception& e) {}

	return true;
}

template<typename T>
bool test_assignment(tools::options_menu<T>& _menu) {

	try {
		//We can set some values right away... string, bool and int values are
		//straightforward: just use the proper argument type...
		_menu.set("22_NAME", "Bob Ross");
		_menu.set("25_FILESIZE", 1024);
		_menu.set("27_BACKUP", false);

		//Choice values are not difficult either as long as we assign by with the
		//correct type...
		_menu.set("20_HELP", false);
		_menu.set("40_PERIODICITY", 4);
		_menu.set("10_WINDOW", "1200x750");
		_menu.set("15_FILTER", std::string("anisotropic"));

		return true;
	}
	catch(std::exception& e) {
		std::cout<<"error: "<<e.what()<<std::endl;
		return false;
	}
}

template<typename T>
bool test_recovery(tools::options_menu<T>& _menu) {

	try {
		if(_menu.get_string("22_NAME")!="Bob Ross") {
			std::cout<<"string recovery failed"<<std::endl;
			return false;
		}

		if(1024!=_menu.get_int("25_FILESIZE")) {
			std::cout<<"int recovery failed"<<std::endl;
			return false;
		}

		if(false!=_menu.get_bool("27_BACKUP")) {
			std::cout<<"bool recovery failed "<<std::endl;
			return false;
		}

		if(false!=_menu.get_bool("20_HELP")) {
			std::cout<<"bool choice recovery failed"<<std::endl;
			return false;
		}

		if(4!=_menu.get_int("40_PERIODICITY")) {
			std::cout<<"int choice recovery failed"<<std::endl;
			return false;
		}

		if(_menu.get_string("10_WINDOW")!="1200x750") {
			std::cout<<"string choice recovery failed with std string"<<std::endl;
			return false;
		}

		if(_menu.get_string("15_FILTER")!="anisotropic") {
			std::cout<<"string choice recovery failed with std string"<<std::endl;
			return false;
		}

		return true;
	}
	catch(std::exception& e) {
		std::cout<<"error: "<<e.what()<<std::endl;
		return false;
	}
}

template<typename T>
bool test_erase(tools::options_menu<T>& _menu) {

	try {
		if(!_menu.key_exists("35_DELETE")) {
			throw std::runtime_error("key should exist before deletion");
		}

		_menu.erase("35_DELETE");

		if(_menu.key_exists("35_DELETE")) {
			throw std::runtime_error("key should not exist before deletion");
		}

		if(8!=_menu.size()) {
			throw std::runtime_error("size should have adapted");
		}

		return true;
	}
	catch(std::exception& e) {
		std::cout<<"error: "<<e.what()<<std::endl;
		return false;
	}
}

template<typename t>
bool test_choice_manipulation(tools::options_menu<t>& _menu) {

	try {
		if(3!=_menu.size_choice("10_WINDOW")) {
			throw std::runtime_error("invalid size for 10_WINDOW");
		}

		if(5!=_menu.size_choice("15_FILTER")) {
			throw std::runtime_error("invalid size for 15_FILTER");
		}

		if(2!=_menu.size_choice("20_HELP")) {
			throw std::runtime_error("invalid size for 20_HELP");
		}

		if(4!=_menu.size_choice("40_PERIODICITY")) {
			throw std::runtime_error("invalid size for 40_PERIODICITY");
		}

		//Try some invalid erases: these should fail because of type mismatches.
		try {
			_menu.erase_choice("40_PERIODICITY", "invalid");
			std::cout<<"error: invalid erase"<<std::endl;
			return false;
		}
		catch(std::exception& e) {}

		try {
			_menu.erase_choice("40_PERIODICITY", false);
			std::cout<<"error: invalid erase"<<std::endl;
			return false;
		}
		catch(std::exception& e) {}

		try {
			_menu.erase_choice("20_HELP", 33);
			std::cout<<"error: invalid erase"<<std::endl;
			return false;
		}
		catch(std::exception& e) {}

		//Empty an option
		_menu.erase_choice("40_PERIODICITY", 1);
		_menu.erase_choice("40_PERIODICITY", 2);
		_menu.erase_choice("40_PERIODICITY", 4);
		_menu.erase_choice("40_PERIODICITY", 8);

		if(0!=_menu.size_choice("40_PERIODICITY")) {
			throw std::runtime_error("option should be empty...");
		}

		//Try some invalid operations with empty choices.
		try {
			_menu.get_str_value("40_PERIODICITY");
			std::cout<<"error: empty choice should not allow str_value to be retrieved"<<std::endl;
			return false;
		}
		catch(std::exception& e) {}
		
		try {
			_menu.get_int("40_PERIODICITY");
			std::cout<<"error: empty choice should not allow value to be retrieved"<<std::endl;
			return false;
		}
		catch(std::exception& e) {}

		try {
			_menu.browse("40_PERIODICITY", tools::options_menu<t>::browse_dir::next);
			std::cout<<"error: empty choice should not allow value to be retrieved"<<std::endl;
			return false;
		}
		catch(std::exception& e) {}

		//Try adding invalid types...
		try {
			_menu.add("40_PERIODICITY", false);
			std::cout<<"error: invalid add"<<std::endl;
			return false;
		}
		catch(std::exception& e) {}

		try {
			_menu.add("40_PERIODICITY", "test");
			std::cout<<"error: invalid add"<<std::endl;
			return false;
		}
		catch(std::exception& e) {}

		try {
			_menu.add("20_HELP", 33);
			std::cout<<"error: invalid add"<<std::endl;
			return false;
		}
		catch(std::exception& e) {}

		//Adding real values to recompose the menu
		for(int _val : std::vector<int>{1,2,4,8}) {
			_menu.add("40_PERIODICITY", _val);
		}

		return true;
	}
	catch(std::exception& e) {
		std::cout<<"error: "<<e.what()<<std::endl;
		return false;
	}
}

int main(int, char **) {

	try {
		//Get the json node...
		auto json_document=tools::parse_json_string(
			tools::dump_file("menu.json")
		);

		//Build the menu...
		std::cout<<"Testing menu building..."<<std::endl;
		tools::options_menu<std::string> menu_str;
		tools::options_menu_from_json(
			json_document["string_string_menu"],
			menu_str
		);

		std::cout<<"testing size..."<<std::endl;
		if(9!=menu_str.size()) {
			std::cout<<"size failed..."<<std::endl;
			return 1;
		}

		std::cout<<"testing erase..."<<std::endl;
		if(!test_erase(menu_str)) {
			std::cout<<"erase failed..."<<std::endl;
			return 1;
		}

		std::cout<<"testing choice manipulation..."<<std::endl;
		if(!test_choice_manipulation(menu_str)) {
			std::cout<<"choice manipulation failed..."<<std::endl;
			return 1;
		}

		std::cout<<"Testing assignment..."<<std::endl;
		if(!test_assignment(menu_str)) {
			std::cout<<"assigments failed..."<<std::endl;
			return 1;
		}

		//Of course, we can make it fail if we assign values of invalid types...
		std::cout<<"Testing invalid assignment..."<<std::endl;
		if(!test_assignment_failures(menu_str)) {
			std::cout<<"invalid assigments succeded..."<<std::endl;
			return 1;
		}

		std::cout<<"Testing recovery..."<<std::endl;
		if(!test_recovery(menu_str)) {
			std::cout<<"recovery failed..."<<std::endl;
			return 1;
		}

		//TODO: Test browsing.

		//This menu has not been translated, as the following lines shows.
		std::cout<<"Untranslated menu:"<<std::endl;
		show_menu(menu_str);
/*

		//Now we can do some sort of interactive demo...
		while(true) {
			std::cout<<"use wasd + enter to navigate:"<<std::endl;
			show_menu(menu_str);
			break;
		}

		//This will show that another types of menu will compile...
		//TODO: Build another menu, int to str, just to see that it compiles.
*/
		return 0;
	}
	catch(std::exception& e) {

		std::cout<<"error: "<<e.what()<<std::endl;
	}
}
