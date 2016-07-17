#ifndef TOOLS_OPTIONS_MENU_H
#define TOOLS_OPTIONS_MENU_H

#include <iostream>

#include <type_traits>
#include <string>
#include <map>
#include <vector>
#include <exception>
#include <algorithm>
#include <memory>

#include "ranged_value.h"
#include "compatibility_patches.h"

#include "../class/dnot_parser.h"

/*
La representación interna de un menú de options de un único nivel en el que
cada opción puede tener N valuees que van "rotando", por ejemplo:

menu:
	resolución de pantalla : [800x600], [1000x1000]
	volumen_audio : [Sin sonido, Medio, Alto]
	idioma: [Español, Inglés, Francés, Japonés]
	cantidad: [un número entre 1 y 99]

El menu sería el primer nivel. El segundo nivel "resolución de pantalla",
"volumen_audio" e "idioma" serían "options" y el último nivel serían 
"selections"

Cada opción y selección están identificados por una key que no debe repetirse.
En caso de que se repita tendremos una excepción. Ojo, no debe repetirse en 
general, no sólo entre options y selections!!!.

A la hora de insertar tanto una opción como una key se ordenarán por el tipo 
Tkey especificado en el template.

Es perfectamente posible tener selections dentro de una opción con el mismo 
value.

Va acompañado de un método "translate" que hace una pasada simple por todos los
"names" para replacelos por otro, de forma que podamos hacer menús 
en múltiples idiomas.

Al final de este archivo hay un ejemplo de uso.

--== Interface pública ==-------------------------------------------------------

Por lo general todos los métodos lanzan excepción si la key no existe.

//TODO: Recrear interface.
//TODO: Recrear ejemplo.
*/

namespace tools
{

class options_menu_exception:
	public std::runtime_error
{
	public:
	options_menu_exception(const std::string& s):std::runtime_error(s) {}
};

template<typename Tkey>
class options_menu
{
	public:

	struct translation_struct
	{
		Tkey 		search;
		std::string 	replace_value;

		void 		replace(const Tkey& key, std::string& cad) const
		{
			if(key==search) 
			{
				cad=replace_value;
			}
		};
	};

	private:

	enum class types {ttemplated, tint, tbool, tstring, tvoid};

	struct base_selection
	{
		std::string 			name;

		virtual	std::string		get_title() const=0;
		virtual types			get_type() const=0;
		virtual void			browse(int)=0;
		virtual void			translate(const translation_struct& t)=0;
						base_selection(const std::string& n):name(n){}
	};

	typedef std::unique_ptr<base_selection>	uptr_base;

	//Estructura para un value del tipo Tvalue, representado como string.
	//Es un template.
	template<typename Tvalue>
	struct option_menu_templated:public base_selection
	{
		//El tipo de una selección de menú de tipo string.
		struct selection_menu_templated
		{		
			Tvalue			value;
			std::string		name; 
		};

		std::map<Tkey, selection_menu_templated>	selections;
		Tkey				current_key;

		Tvalue				get_value() const 
		{
			check_options("option with no selections for get_value");
			return selections.at(current_key).value;
		}
		virtual std::string		get_title() const 
		{
			check_options("option with no selections for get_title");
			return selections.at(current_key).name;
		}

		virtual types				get_type() const {return types::ttemplated;}

		virtual void 				browse(int dir)
		{
			auto it=selections.find(current_key);

			//Descender...
			if(dir < 0)
			{
				if(it==selections.begin()) //Dar la vuelta.
				{
					current_key=selections.rbegin()->first;
				}
				else
				{
					current_key=std::prev(it)->first;
				}
			}
			//Ascender...
			else
			{
				auto sigue=std::next(it);
				if(sigue==std::end(selections))
				{
					current_key=selections.begin()->first;
				}
				else
				{
					current_key=sigue->first;
				}
			}
		}

		void 					set_by_value(const Tvalue& value)
		{
			for(auto& seleccion : selections)
			{
				if(seleccion.second.value==value) 
				{
					current_key=seleccion.first;
					return;
				}
			}

			throw options_menu_exception("value does not exist for set_by_value");
		}

		void					set_by_key(const Tkey& key_sel)
		{
			if(!selections.count(key_sel)) 
			{
				throw options_menu_exception("key does not exist for set_by_key");
			}
			
			current_key=key_sel;
		}

		size_t				size() const
		{
			return selections.size();
		}

		void				check_options(const std::string& msg) const
		{
			if(!selections.size()) throw options_menu_exception(msg);
		}

		void				insert(const Tkey& key, const Tvalue& value, const std::string& name)
		{
			if(!selections.size()) current_key=key;
			selections[key]={value, name};
		}

		void 				erase(const Tkey& key_sel)
		{
			if(!selections.count(key_sel)) throw options_menu_exception("key does not exist for erase");
			selections.erase(key_sel);
		}

		virtual void			translate(const translation_struct& t)
		{
			for(auto& seleccion : selections)
			{
				t.replace(seleccion.first, seleccion.second.name);
			}
		}

		option_menu_templated(const std::string& n):base_selection(n), current_key() {}
	};

	//Estructura para la selección de un value de enteros.
	struct option_menu_int:public base_selection
	{
		ranged_value<int>		value;

		virtual std::string		get_title() const {return compat::to_string(value.get());}
		int				get_value() const {return value.get();}
		virtual types			get_type() const {return types::tint;}
		virtual void			browse(int d){value+=d;}
		virtual void			translate(const translation_struct& t){}

		option_menu_int(const std::string& n, int min, int max, int a):base_selection(n), value(min, max, a) {} 
	};

	//Estructura para la selección de un value booleano.
	struct option_menu_bool:public base_selection
	{
		bool				value;	
	
		virtual std::string		get_title() const {return compat::to_string(value);}
		bool				get_value() const {return value;}
		virtual types			get_type() const {return types::tbool;}
		virtual void			browse(int d){value=!value;}
		virtual void			translate(const translation_struct& t){}

		option_menu_bool(const std::string& n, bool v):base_selection(n), value(v) {} 
	};

	//Estructura para la selección de un value string abierto.
	struct option_menu_string:public base_selection
	{
		std::string			value;
	
		std::string			get_value() const {return value;}
		virtual std::string		get_title() const {return value;}
		virtual types			get_type() const {return types::tstring;}
		virtual void			browse(int d){}
		virtual void			translate(const translation_struct& t){}

		option_menu_string(const std::string& n, const std::string& v):base_selection(n), value(v) {} 
	};

	//Estructura para la selección de una entrada sin value.
	struct option_menu_void:public base_selection
	{
		void				get_value() const {}
		virtual std::string		get_title() const {return "";}
		virtual types			get_type() const {return types::tvoid;}
		virtual void			browse(int d){}
		virtual void			translate(const translation_struct& t){}

		option_menu_void(const std::string& n):base_selection(n) {} 
	};

	void	check_option(const Tkey& key, const std::string& msg) const
	{
		if(!options.count(key)) 
			throw options_menu_exception(msg);
	}

	void	check_unique_key(const Tkey& key) const
	{
		if(std::find(std::begin(keys), std::end(keys), key)!=std::end(keys))
		{
			throw options_menu_exception("duplicate key detected");
		}

		keys.push_back(key);
	}

	void	validate_type(types ot, types t, const std::string& msg) const
	{
		if(ot!=t) throw new options_menu_exception(msg);
	}

	public:

	void		insert_int(const Tkey& key, const std::string& name, int min, int max, int act)
	{
		check_unique_key(key);
		options.insert(std::pair<Tkey, uptr_base>(key, uptr_base(new option_menu_int(name, min, max, act))));
	}

	void		insert_bool(const Tkey& key, const std::string& name, bool val)
	{
		check_unique_key(key);
		options.insert(std::pair<Tkey, uptr_base>(key, uptr_base(new option_menu_bool(name, val))));
	}

	void		insert_string(const Tkey& key, const std::string& name, const std::string& val)
	{
		check_unique_key(key);
		options.insert(std::pair<Tkey, uptr_base>(key, uptr_base(new option_menu_string(name, val))));
	}

	void		insert_void(const Tkey& key, const std::string& name)
	{
		check_unique_key(key);
		options.insert(std::pair<Tkey, uptr_base>(key, uptr_base(new option_menu_void(name))));
	}

	void		erase(const Tkey& key)
	{
		check_option(key, "key does not exist for erase");
		options.erase(key);
	}

	//Para rotatorias representadas como string.
	template<typename Tvalue>
	void		insert_templated(const Tkey& key, const std::string& name)
	{
		check_unique_key(key);
		options.insert(std::pair<Tkey, uptr_base>(key, uptr_base(new option_menu_templated<Tvalue>(name))));
	}

	template<typename Tvalue>
	void		insert_templated(const Tkey& key, const Tkey& key_sel, const std::string& name, const Tvalue& value)
	{
		check_unique_key(key_sel);
		check_option(key, "key does not exist for insert template: "+name);
		auto& o=options.at(key);
		validate_type(o->get_type(), types::ttemplated, "option is not templated");
		static_cast<option_menu_templated<Tvalue> *>(o.get())->insert(key_sel, value, name);
	}

	template<typename Tvalue>
	void		erase_templated(const Tkey& key, const Tkey& key_sel)
	{
		check_option(key, "key does not exist for erase template");
		auto& o=options.at(key);
		validate_type(o->get_type(), types::ttemplated, "option is not templated");
		static_cast<option_menu_templated<Tvalue> *>(o.get())->erase(key_sel);
	}

	template<typename Tvalue>
	void		set_by_key_templated(const Tkey& key, const Tkey& key_sel)
	{
		check_option(key, "key does not exist for set_by_key templated");
		auto& o=options.at(key);
		validate_type(o->get_type(), types::ttemplated, "option is not templated");
		static_cast<option_menu_templated<Tvalue> *>(o.get())->set_by_key(key_sel);
	}

	template<typename Tvalue>
	void		set_by_value_templated(const Tkey& key, const Tvalue& value)
	{
		check_option(key, "key does not exist for set_by_value templated");
		auto& o=options.at(key);
		validate_type(o->get_type(), types::ttemplated, "option is not templated");
		static_cast<option_menu_templated<Tvalue> *>(o.get())->set_by_value(value);
	}

	template<typename Tvalue>
	Tvalue		get_value_templated(const Tkey& key) const
	{
		check_option(key, "key does not exist for get_value templated");
		const auto& o=options.at(key);
		validate_type(o->get_type(), types::ttemplated, "option is not templated");
		return static_cast<option_menu_templated<Tvalue> *>(o.get())->get_value();
	}

	template<typename Tvalue>
	size_t		size_templated(const Tkey& key) const
	{
		check_option(key, "key does not exist for size templated");
		auto& o=options.at(key);
		validate_type(o->get_type(), types::ttemplated, "option is not templated");
		return static_cast<option_menu_templated<Tvalue> *>(o.get())->size();
	}

	void		browse(const Tkey& key, int dir)
	{
		check_option(key, "key does not exist for browse");
		options.at(key)->browse(dir);
	}

	//Para el tipo int.
	int	get_int(const Tkey& key) const
	{
		check_option(key, "key does not exist for get_int");
		const auto& o=options.at(key);
		validate_type(o->get_type(), types::tint, "option is not int");
		return static_cast<option_menu_int *>(o.get())->get_value();
	}

	void	set_int(const Tkey& key, int value)
	{
		check_option(key, "key does not exist for set_int");
		auto& o=options.at(key);
		validate_type(o->get_type(), types::tint, "option is not int");
		static_cast<option_menu_int *>(o.get())->value=value;
	}

	//Para el tipo bool.
	bool	get_bool(const Tkey& key) const
	{
		check_option(key, "key does not exist for get_bool");
		const auto& o=options.at(key);
		validate_type(o->get_type(), types::tbool, "option is not bool");
		return static_cast<option_menu_bool *>(o.get())->get_value();
	}

	void	set_bool(const Tkey& key, bool value)
	{
		check_option(key, "key does not exist for set_bool");
		auto& o=options.at(key);
		validate_type(o->get_type(), types::tbool, "option is not bool");
		static_cast<option_menu_bool *>(o.get())->value=value;
	}

	//Para el tipo string.
	std::string	get_string(const Tkey& key) const
	{
		check_option(key, "key does not exist for get_string");
		const auto& o=options.at(key);
		validate_type(o->get_type(), types::tstring, "option is not string");
		return static_cast<option_menu_string *>(o.get())->get_value();
	}

	void	set_string(const Tkey& key, const std::string& value)
	{
		check_option(key, "key does not exist for set_string");
		auto& o=options.at(key);
		validate_type(o->get_type(), types::tstring, "option is not string");
		static_cast<option_menu_string *>(o.get())->value=value;
	}

	std::string	get_name(const Tkey& key) const
	{
		check_option(key, "key does not exist for get_name");
		return options.at(key)->name;
	}

	std::string	get_title(const Tkey& key) const
	{
		check_option(key, "key does not exist for get_title");
		const auto& o=options.at(key);
		return o->get_title();
	}


	void		translate(const std::vector<translation_struct>& v)
	{
		for(const auto& t : v) 
		{
			for(auto& opcion : options)
			{
				t.replace(opcion.first, opcion.second->name);
				opcion.second->translate(t);
			}
		}
	}

	size_t		size() const
	{
		return options.size();
	}	

	std::vector<Tkey>		get_keys() const 
	{
		//No podemos usar "keys" puesto que contiene también las keys de las selections...
		std::vector<Tkey> res;
		for(const auto& o : options) res.push_back(o.first);
		return res;
	}

	private:

	mutable std::vector<Tkey>				keys;
	std::map<Tkey, std::unique_ptr<base_selection>>		options;
};

template<typename Tkey>
void options_menu_from_dnot(
	const std::string& filename, 
	const std::string& root, 
	options_menu<Tkey>& data, //This should be a reference to the real menu.
	std::map<Tkey, int>& translation_map) //This seems to be a reference to the real translation map
{
	const auto parser=dnot_parse(filename);
	const auto options=parser[root].get_vector();

	for(const auto& opcion : options)
	{
		const std::string k_opcion=opcion["c"];
		translation_map[k_opcion]=opcion["t"];

		const std::string tipo_menu=opcion["m"];

		if(tipo_menu=="templated")
		{
			//Bufff... Ese "template" está para que el compilador no se grille: es el "template disambiguator", 
			//que ayuda a saber que es un método templatizado y no una propiedad seguida de "menor que".

			const std::string mt=opcion["mt"];
			if(mt=="string") data.template insert_templated<std::string>(k_opcion, "-");
			else if(mt=="int") data.template insert_templated<int>(k_opcion, "-");
			else if(mt=="bool") data.template insert_templated<bool>(k_opcion, "-");
			else throw std::runtime_error("unknown template type "+mt);

			const auto& selections=opcion["o"].get_vector();
			for(const auto& seleccion : selections)
			{
				const std::string k_seleccion=seleccion["c"];
				translation_map[k_seleccion]=seleccion["t"];

				if(mt=="string") data.template insert_templated<std::string>(k_opcion, k_seleccion, "-", seleccion["v"]);
				else if(mt=="int") data.template insert_templated<int>(k_opcion, k_seleccion, "-", seleccion["v"]); 
				else if(mt=="bool") data.template insert_templated<bool>(k_opcion, k_seleccion, "-", seleccion["v"]);
			}
		}
		else if(tipo_menu=="int")
		{
			data.insert_int(k_opcion, "-", opcion["min"], opcion["max"], opcion["min"]);
		}
		else if(tipo_menu=="bool")
		{
			data.insert_bool(k_opcion, "-", true);
		}
		else if(tipo_menu=="string")
		{
			data.insert_string(k_opcion, "-", "-");
		}
		else if(tipo_menu=="void")
		{
			data.insert_void(k_opcion, "-");
		}
	}
}

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

#endif
