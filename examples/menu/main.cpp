#include <iostream>
#include <tools/options_menu.h>
#include <tools/json.h>

#include <rapidJson/document.h>

#include <vector>
#include <string>
#include <map>

int main(int, char **) {

	auto json_document=tools::parse_json_string(
		tools::dump_file("menu_string_string.json")
	);
	
	tools::options_menu<std::string> menu_a{};
	std::map<std::string, std::string> menu_a_translation_map{};

	tools::options_menu_from_json(
		document["string_string_menu"],
		menu_a,
		&menu_a_translation_map
	);

	//TODO: WTF?????

	return 0;
}
