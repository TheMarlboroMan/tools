#include <iostream>
#include <tools/options_menu.h>
#include <tools/json.h>
#include <tools/file_utils.h>

#include <rapidjson/document.h>

#include <vector>
#include <string>
#include <map>

int main(int, char **) {

	//Get the json node...
	auto json_document=tools::parse_json_string(
		tools::dump_file("menu.json")
	);
	
	//Build the menu...
	tools::options_menu<std::string> menu_str_str;

	//Build a translator from the key type (string) to the translation 
	//type (int).
	tools::options_menu_translator<std::string, int> translator_str_str;

	//There are two opposite sides of data for the translator. One is outside
	//the menu, where the data strings are found...
	for(const auto& entry : json_document["translation_pairs_english"].GetArray()) {
		translator_str_str.add_id_to_text(entry["index"].GetInt(), entry["value"].GetString());
	}

	//...and the other is in the menu, where translation id resides. Building
	//the menu does build this data too.
	//TODO: Horrible. Should be a separate operation.
	tools::options_menu_from_json(
		json_document["string_string_menu"],
		menu_str_str,
		&translator_str_str
	);

	//And finally, translate.
	menu_str_str.translate(translator_str_str);

	return 0;
}
