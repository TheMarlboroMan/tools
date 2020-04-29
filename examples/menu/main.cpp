#include <iostream>
#include <tools/options_menu.h>
#include <tools/json.h>
#include <tools/file_utils.h>

#include <rapidjson/document.h>

#include <vector>
#include <string>
#include <map>
#include <functional>

template<typename T>
void show_menu(const tools::options_menu<T>& _menu, T _current_key) {

	//The menu does not provide any type of translation for keys...
	//Client code must provide its own.

	std::map<T, std::string> key_to_title {
		{"10_WINDOW", "window size"},
		{"15_FILTER", "texture filtering"},
		{"20_HELP", "help blurbs"},
		{"40_PERIODICITY", "backup periodicity"},
		{"22_NAME", "process username"},
		{"25_FILESIZE", "max filesize of data file (in MB)"},
		{"26_COUNT", "warning countdown (in seconds)"},
		{"27_BACKUP", "backup active"},
		{"30_EXIT", "save and exit"},
	};

	auto from_int=[&_menu](const T& _key) -> std::string {return std::to_string(_menu.get_int(_key));};
	auto from_bool=[&_menu](const T& _key) -> std::string {return _menu.get_bool(_key) ? "yes" : "no";};
	auto from_string=[&_menu](const T& _key) -> std::string {return _menu.get_string(_key);};
	auto from_void=[&_menu](const T&) -> std::string {return "";};

	//Translations of values are not provided either!
	std::map<T, std::function<std::string(const T&)>> key_to_value {
		{"10_WINDOW", from_string},
		{"15_FILTER", from_string},
		{"20_HELP", from_bool},
		{"40_PERIODICITY", from_int},
		{"22_NAME", from_string},
		{"25_FILESIZE", from_int},
		{"26_COUNT", from_int},
		{"27_BACKUP", from_bool},
		{"30_EXIT", from_void},
	};

	for(const auto& key : _menu.get_keys()) {

		auto title=key_to_title[key];
		auto value=key_to_value[key](key);

		if(key==_current_key) {
			std::cout<<"["<<title<<"] : "<<value<<std::endl;
		}
		else {
			std::cout<<" "<<title<<"  : "<<value<<std::endl;
		}
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

	//Recovering values is done through get_#type. The value must be of the
	//same type. So far there's no way to inspect an entry and get its type.
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

		if(9!=_menu.size()) {
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
			_menu.get_int("40_PERIODICITY");
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

template<typename t>
bool test_browse(tools::options_menu<t>& _menu) {

	using m=tools::options_menu<t>;

	//test non browsable
	try {
		_menu.browse("22_NAME", m::browse_dir::next);
		std::cout<<"error: string should not be browsable"<<std::endl;
		return false;
	}
	catch(std::exception&) {}

	try {
		_menu.browse("30_EXIT", m::browse_dir::next);
		std::cout<<"error: void should not be browsable"<<std::endl;
		return false;
	}
	catch(std::exception&) {}

	//test empty browse
	_menu.insert("60_TEST", std::vector<int>{}, false);
	try {
		_menu.browse("60_TEST", m::browse_dir::next);
		std::cout<<"error: empty choice should not allow value to be browsed"<<std::endl;
		return false;
	}
	catch(std::exception& e) {
		_menu.erase("60_TEST");
	}

	//test browse.
	_menu.set("40_PERIODICITY", 1);
	for(const int val : std::vector<int>{1,2,4,8,1}) {
		if(val!=_menu.get_int("40_PERIODICITY")) {
			throw std::runtime_error("unexpected browse result in choice forward motion");
		}

		_menu.browse("40_PERIODICITY", m::browse_dir::next);
	}

	_menu.set("40_PERIODICITY", 1);
	for(const int val : std::vector<int>{1,8,4,2,1}) {
		if(val!=_menu.get_int("40_PERIODICITY")) {
			throw std::runtime_error("unexpected browse result in choice backwards motion");
		}

		_menu.browse("40_PERIODICITY", m::browse_dir::previous);
	}

	_menu.set("27_BACKUP", false);
	for(const bool val : std::vector<bool>{false, true, true}) {
		if(val!=_menu.get_bool("27_BACKUP")) {
			throw std::runtime_error("unexpected browse result in bool forward motion");
		}

		_menu.browse("27_BACKUP", m::browse_dir::next);
	}

	_menu.set("27_BACKUP", true);
	for(const bool val : std::vector<bool>{true, false, false}) {
		if(val!=_menu.get_bool("27_BACKUP")) {
			throw std::runtime_error("unexpected browse result in bool backwards motion");
		}

		_menu.browse("27_BACKUP", m::browse_dir::previous);
	}

	_menu.set("25_FILESIZE", 9998);
	for(const int val : std::vector<int>{9998, 9999, 1, 2}) {
		if(val!=_menu.get_int("25_FILESIZE")) {
			throw std::runtime_error("unexpected browse result in int forward motion");
		}

		_menu.browse("25_FILESIZE", m::browse_dir::next);
	}

	_menu.set("25_FILESIZE", 2);
	for(const int val : std::vector<int>{2, 1, 9999, 9998}) {
		if(val!=_menu.get_int("25_FILESIZE")) {
			throw std::runtime_error("unexpected browse result in int backwards motion");
		}

		_menu.browse("25_FILESIZE", m::browse_dir::previous);
	}

	_menu.set("26_COUNT", 10);
	for(const int val : std::vector<int>{10, 11, 12, 13, 14, 15, 15}) {
		if(val!=_menu.get_int("26_COUNT")) {
			throw std::runtime_error("unexpected browse result in bound int forward motion");
		}

		_menu.browse("26_COUNT", m::browse_dir::next);
	}

	for(const int val : std::vector<int>{15, 14, 13, 12, 11, 10, 10}) {
		if(val!=_menu.get_int("26_COUNT")) {
			throw std::runtime_error("unexpected browse result in bound int backwards motion");
		}

		_menu.browse("26_COUNT", m::browse_dir::previous);
	}

	return true;
}

template<typename t>
bool test_adjacent_keys(tools::options_menu<t>& _menu) {

	try {

		//These are the remanining keys...
		std::vector<std::string> keys={
			"10_WINDOW",
			"15_FILTER",
			"20_HELP",
			"40_PERIODICITY",
			"22_NAME",
			"25_FILESIZE",
			"26_COUNT",
			"27_BACKUP",
			"30_EXIT"
		};

		//...should be the same as the menu keys.
		if(keys!=_menu.get_keys()) {

			throw std::runtime_error("keys do not match");
		}

		using m=tools::options_menu<t>;

		//Try first non bounding mode.
		_menu.set_wrap(false);
		if(keys.front()!=_menu.adjacent_key("10_WINDOW", m::browse_dir::previous)) {

			throw std::runtime_error("keys do not match when retrieving previous key (first) in bounding mode");
		}

		if(keys.front()!=_menu.adjacent_key("15_FILTER", m::browse_dir::previous)) {

			throw std::runtime_error("keys do not match when retrieving previous key (second) in bounding mode");
		}

		if(keys.back()!=_menu.adjacent_key("30_EXIT", m::browse_dir::next)) {

			throw std::runtime_error("keys do not match when retrieving next key (last) in bounding mode");
		}

		if(keys.back()!=_menu.adjacent_key("27_BACKUP", m::browse_dir::next)) {

			throw std::runtime_error("keys do not match when retrieving next key (previous to last) in bounding mode");
		}

		//Try restore bounding mode.
		_menu.set_wrap(true);
		int i=0;
		for(; i < (int)keys.size(); i++) {

			const auto current_key=keys[i];

			if(i==0) {
				if(keys.back()!=_menu.adjacent_key(current_key, m::browse_dir::previous)) {
					throw std::runtime_error("keys do not match when retrieving key previous to first in non-bounding mode");
				}

				if(keys[1]!=_menu.adjacent_key(current_key, m::browse_dir::next)) {
					throw std::runtime_error("keys do not match when retrieving key next to first in non-bounding mode");
				}
			}
			else if(i==8) {

				if(keys[7]!=_menu.adjacent_key(current_key, m::browse_dir::previous)) {
					throw std::runtime_error("keys do not match when retrieving key previous to last in non-bounding mode");
				}

				if(keys.front()!=_menu.adjacent_key(current_key, m::browse_dir::next)) {
					throw std::runtime_error("keys do not match when retrieving key next to last in non-bounding mode");
				}
			}
			else {

				if(keys[i-1]!=_menu.adjacent_key(current_key, m::browse_dir::previous)) {

					throw std::runtime_error("keys do not match when retrieving next key in non-bounding mode");
				}

				if(keys[i+1]!=_menu.adjacent_key(current_key, m::browse_dir::next)) {

					throw std::runtime_error("keys do not match when retrieving previous key in non-bounding mode");
				}
			}
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
			json_document["string_menu"],
			menu_str
		);

		std::cout<<"testing size..."<<std::endl;
		if(10!=menu_str.size()) {
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

		std::cout<<"Testing browse..."<<std::endl;
		if(!test_browse(menu_str)) {
			std::cout<<"browse failed..."<<std::endl;
			return 1;
		}

		std::cout<<"Testing adjacent keys..."<<std::endl;
		if(!test_adjacent_keys(menu_str)) {
			std::cout<<"adjacent keys failed..."<<std::endl;
			return 1;
		}

		//Now we can do some sort of interactive demo...

		std::string current_key{"10_WINDOW"};
		bool running=true;
		while(running) {
			std::cout<<"use wasd + enter to navigate:"<<std::endl;

			show_menu(menu_str, current_key);
			std::cout<<">>";

			char user_input=0;
			std::cin>>user_input;
			if(std::cin.fail()) {

				std::cin.clear();
				std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
			}

			switch(user_input) {
				case 'w':
					current_key=menu_str.adjacent_key(current_key, decltype(menu_str)::browse_dir::previous);
				break;
				case 's':
					current_key=menu_str.adjacent_key(current_key, decltype(menu_str)::browse_dir::next);
				break;
				case 'a':
					if(current_key=="30_EXIT") {
						running=false;
						break;
					}

					if(current_key=="22_NAME") {
						break;
					}

					menu_str.browse(current_key, decltype(menu_str)::browse_dir::previous);

				break;
				case 'd':
					if(current_key=="30_EXIT") {
						running=false;
						break;
					}

					if(current_key=="22_NAME") {
						break;
					}

					menu_str.browse(current_key, decltype(menu_str)::browse_dir::next);
				break;
				default:
					std::cout<<"[invalid input]"<<std::endl;
				break;
			}

		}

		//This will show that another types of menu will compile...
		tools::options_menu<int> menu_int;
		tools::options_menu_from_json(
			json_document["int_menu"],
			menu_int
		);

		return 0;
	}
	catch(std::exception& e) {

		std::cout<<"unexpected error: "<<e.what()<<std::endl;
	}
}
