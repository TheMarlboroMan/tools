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

		std::cout<<_menu.get_name(key)<<" : "<<_menu.get_str_value(key)<<std::endl;
	}
}

template<typename T>
bool test_assignment_failures(tools::options_menu<T>& _menu) {

	bool ok=true;

	try {
		_menu.set("22_NAME", 22);
		std::cout<<"22_NAME failed"<<std::endl;
		ok=false;
	}
	catch(std::exception& e) {}

	try {
		_menu.set("25_FILESIZE", "hello");
		std::cout<<"25_FILESIZE failed"<<std::endl;
		ok=false;
	}
	catch(std::exception& e) {}

	try {
		//TODO: Interesting... if we use a const char *, an implicit conversion
		//is made. Perhaps we could use some type traits
/*
template<typename T> struct is_int{static const bool value=false;};
template<> struct is_int<int>{static const bool value=true;};


and then...
static_assert(is_int<tvalue>::value, "argument must be an integer)
*/
		std::cout<<"SETTING BACKUP!"<<std::endl;
		_menu.set("27_BACKUP", "how are you");
		std::cout<<"27_BACKUP failed"<<std::endl;
		ok=false;
	}
	catch(std::exception& e) {}

	try {
	//TODO: Interesting...
	std::cout<<"SETTING HELP!"<<std::endl;
		_menu.set("20_HELP", "in this fine day");
		std::cout<<"20_HELP failed"<<std::endl;
		ok=false;
	}
	catch(std::exception& e) {}

	try {
		//TODO: Interesting...
		_menu.set("40_PERIODICITY", "sir?");
		std::cout<<"40_PERIODICITY failed"<<std::endl;
		ok=false;
	}
	catch(std::exception& e) {}

	try {
		_menu.set("10_WINDOW", false);
		std::cout<<"10_WINDOW failed"<<std::endl;
		ok=false;
	}
	catch(std::exception& e) {}

	return ok;
}

int main(int, char **) {

	try {
		//Get the json node...
		auto json_document=tools::parse_json_string(
			tools::dump_file("menu.json")
		);

		//Build the menu...
		tools::options_menu<std::string> menu_str_str;
		tools::options_menu_from_json(
			json_document["string_string_menu"],
			menu_str_str
		);

		//We can se some values right away... string, bool and int values are
		//straightforward: just use the proper argument type...
		menu_str_str.set("22_NAME", "Bob Ross");
		menu_str_str.set("25_FILESIZE", 1024);
		menu_str_str.set("27_BACKUP", false);

		//Choice values are not difficult either as long as we assign by with the
		//correct type...
		menu_str_str.set("20_HELP", false);
		menu_str_str.set("40_PERIODICITY", 4);
		menu_str_str.set("10_WINDOW", std::string("1200x750")); //we could also use const char *.

		//Of course, we can make it fail if we assign values of invalid types...
		if(!test_assignment_failures(menu_str_str)) {
			std::cout<<"assigments failed..."<<std::endl;
//TODO:			return 1;
		}

		//TODO: Assign by key!



		//This menu has not been translated, as the following lines shows.
		std::cout<<"Untranslated menu:"<<std::endl;
		show_menu(menu_str_str);

		//Build a translator from the key type (string) to the translation type (int).
		tools::options_menu_translator<std::string, int> translator_str_str;

		//There are two opposite sides of data for the translator. One is outside
		//the menu, where the data strings are found...
		for(const auto& entry : json_document["translation_pairs_english"].GetArray()) {
			translator_str_str.add_id_to_text(entry["index"].GetInt(), entry["value"].GetString());
		}

		//...and the other is in the menu itself.
		translator_str_str.load_translation_ids(json_document["string_string_menu"]);

		//When the two sides are loaded, the menu can be translated.
		//TODO :See if this throws when not both sides are loaded.
		menu_str_str.translate(translator_str_str);

		//Now we can do some sort of interactive demo...
		while(true) {
			std::cout<<"use wasd + enter to navigate:"<<std::endl;
			show_menu(menu_str_str);
			break;
		}

		//This will show that another types of menu will compile...
		//TODO: Build another menu, int to str, just to see that it compiles.

		return 0;
	}
	catch(std::exception& e) {

		std::cout<<"error: "<<e.what()<<std::endl;
	}
}
