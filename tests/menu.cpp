#include <iostream>
#include <string>


#include "../source/string_utils.cpp"
#include "../class/dnot_parser.cpp"
#include "../class/dnot_token.cpp"
#include "../class/text_reader.cpp"
#include "../templates/options_menu.h"

int main(int argc, char ** argv)
{
	using namespace tools;

	options_menu<std::string>	menu;
	std::map<std::string, int>	translation_map;
	mount_from_dnot(dnot_parse_file("options_menu.dnot")["config_app"], menu, &translation_map);

	//These are our translations: a key for a value.
	std::map<int, std::string> translations={
		{1, "1 - window size"}, {2, "2 - display help"}, {3, "3 - name"}, {4, "4 - maximum filesize"}, {5, "5 - exit"},
		//Selections...
		{100, "800x500 (natural)"}, {101, "1200x750 (scaled)"}, {102, "1600x1000 (scaled)"},
		{103, "hisplay help"}, {104, "hide help"}
	};

	//Now we use the translation map to create a vector of translation stuff...
	std::vector<options_menu<std::string>::translation_struct > trad;

	//See what we need to translate and fill it. Looks crazy but it's easy:
	//the key of "translation_map" matches the key of the menu. The value 
	//matches the key of "translations".
	for(const auto& p: translation_map) trad.push_back({p.first, translations.at(p.second)});

	//And translate...
	menu.translate(trad);

	//Now... string values have a default and useles value so...
	menu.set_string("22_NAME", "this is a manually set value");

	//And now we can make a fun loop...
	std::string input;

	while(true)
	{
		for(const auto& k : menu.get_keys())
		{
			std::cout<<menu.get_name(k)<<" = "<<menu.get_title(k)<<std::endl;
		}

		std::cout<<"menu index + l/r to change the value (example: 1r, 2l)."<<std::endl<<
			"6+string to set string value (example 6Hello)"<<std::endl<<
			"7+int to set integer value (example 7100)"<<std::endl<<
			"5 to exit"<<std::endl
			<<">>";

		std::getline(std::cin, input);

		auto do_lr=[&](const std::string& in, const std::string& key)
		{
			if(input.size() > 1)
			{
				if(input[1]=='l') 	menu.browse(key, -1);
				else if(input[1]=='r') 	menu.browse(key, 1);
			}
		};


		if(input.size())
		{	
			if(input[0]=='1') do_lr(input, "10_WINDOW");
			else if(input[0]=='2') do_lr(input, "20_HELP");
			else if(input[0]=='3') {}
			else if(input[0]=='4') do_lr(input, "25_FILESIZE");
			else if(input[0]=='5') break;
			else if(input[0]=='6' && input.size() > 1) menu.set_string("22_NAME", input.substr(1));
			else if(input[0]=='7' && input.size() > 1) menu.set_int("25_FILESIZE", std::atoi(input.substr(1).c_str()));
		}
	}
	
	return 1;
}

/*

Ejemplo de uso

int main(int argc, char ** argv)
{
	using namespace tools;

	options_menu<std::string, int> menu;

	menu.insertar_opcion(1, "--OPCION_1");
	menu.insert_en_opcion(1, 100, "--VALOR 1.1", "PRIMER VALOR 1");
	menu.insert_en_opcion(1, 101, "--VALOR 1.2", "2 VALOR 1");
	menu.insert_en_opcion(1, 102, "--VALOR 1.3", "3 VALOR 1");

	menu.insertar_opcion(2, "--OPCION_2");
	menu.insert_en_opcion(2, 103, "--VALOR 2.1", "1 VALOR 2");
	menu.insert_en_opcion(2, 104, "--VALOR 2.2", "2 VALOR 2");

	menu.insertar_opcion(3, "--OPCION_3");
	menu.insert_en_opcion(3, 105, "--VALOR 3.1", "1 VALOR 3");
	menu.insert_en_opcion(3, 106, "--VALOR 3.2", "2 VALOR 3");

	std::vector<options_menu<std::string, int>::translation_struct > trad={ 
		{1, "TAMAÑO PANTALLA"}, {2, "VOLUMEN SONIDO"}, {3, "IDIOMA"}, 
		{100, "300x200"}, {101, "600x400"}, {102, "1200x800"},
		{103, "Medio"}, {104, "Alto"}, 
		{105, "Español"}, {106, "Inglés"} 
	};
	menu.translate(trad);

	const auto& v=menu.get_keys();

	int i=0;

	while(i < 4)
	{
		for(auto c : v)
		{
			std::cout<<"C"<<c<<" : "<<menu.name_opcion(c)<<" : "<<menu.name_seleccion(c)<<" : '"<<menu.value_opcion(c)<<"'"<<std::endl;
			menu.browse_opcion(c, 1);
		}

		std::cout<<" ------ "<<std::endl;
		++i;
	}

	return 1;
}
*/
