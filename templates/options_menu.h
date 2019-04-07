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


namespace tools
{

//!Exception thrown by the options_menu class.
class options_menu_exception:
	public std::runtime_error {
	public:
	//!Class constructor.
	options_menu_exception(const std::string& s):std::runtime_error(s) {}
};

//!Data representation of a single depth menu.
/**
This class is built to represent a single depth menu with N options, each
options with N selections, such as this one:

menu:
	screen_resolution : [800x600], [1000x1000]
	sound_volume : [Mute, Low, Mid, High]
	language: [English, Spanish, Japanese]
	lives: [1-99]

The first level, "menu" is the class itself. Each node (screen_resolution, 
sound_volume, language and lives) is a "option" and for each option, different
"selections" are given.

Options and selections are identified by an unique key. This key must be 
completely unique: if shared between, for example, an option and a selection,
an exception will be thrown. The key is the typename of the template.

The order of options and selections is determined by the natural order of the
typename. For the best flexibility, std::strings are recommended.

There are different types of options, each can have N selections with different
values. Values can be repeated within an option. The types of options are:

	templated: used to represent any given type (string, int, bool, usually) with a string.
	int: used to represent integer values within a range.
	bool: used to represent true or false
	string: used to store strings.
	void: does not store anything, like the access to a submenu.

Each option has a name. This name is translatable with the "translate" method.

Menus can be built by code or from a dnot file (this last method is 
actually easier to mantain, of course).

Finally, there is a particular exception "options_menu_exception" thrown for
every error.
*/

template<typename Tkey>
class options_menu
{
	public:

	//!Translation structure that matches the typename key to a string that will be used to name it. Many of these are needed for a translation.

	//!Basically means "when you find this Tkey, give it this string name".
	struct translation_struct {
		Tkey 		search;			//!< Key being searched.
		std::string 	replace_value;		//!< Replacement for value.

		//!Receives a pair of key and value. If the key is the one
		//!being searched (TKey search), its value is replaced by
		//!replace_value.
		void 		replace(const Tkey& key, std::string& cad) const {
			if(key==search) {
				cad=replace_value;
			}
		};
	};

	private:

	//!Defines the different types of selections.
	enum class types {ttemplated, tint, tbool, tstring, tvoid};

	//!Base class for all selections.
	struct base_selection {

		std::string 			name; //!< All selections have a name string.

		//!Will return the name of the current selection (or its equivalent).
		virtual	std::string		get_title() const=0;
		//!Returns its type. Internal use.
		virtual types			get_type() const=0;
		//!Chooses the next/previous value/selection in the option.
		virtual void			browse(int)=0;
		//!Translates the selection. 
		virtual void			translate(const translation_struct& t)=0;
		//Base constructor.
						base_selection(const std::string& n):name(n){}
	};

	
	typedef std::unique_ptr<base_selection>	uptr_base;

	//!Widely used structure to represent any type with a string. 

	//!Consists of N selections (of type Tvalue), each one represented by a
	//!different string. For example, the values 0,1 and 2 (int) could 
	//!repreent "txt", "html" and "latex" output. It is implemented in 
	//!terms of a std::map.

	template<typename Tvalue>
	struct option_menu_templated:public base_selection {
		//!Pair of value and name.
		struct selection_menu_templated {
			Tvalue			value;	//!< Stored value.
			std::string		name; 	//!< Display name.
		};

		std::map<Tkey, selection_menu_templated>	selections; //!< Internal option map.
		Tkey				current_key;	//!< Currently selected key.

		//!Returns the typename value for the current selection (0,1,2 in the example).
		Tvalue				get_value() const {
			check_options("option with no selections for get_value");
			return selections.at(current_key).value;
		}

		//!Returns the string that represents the current selection ("txt", "html" or "latex" in the example).
		virtual std::string		get_title() const {
			check_options("option with no selections for get_title");
			return selections.at(current_key).name;
		}

		//!Returns the type of this class.
		virtual types				get_type() const {return types::ttemplated;}

		//!Changes the current selection. 

		//!Values smaller than zero turn the selection "left" and larger
		//!than zero turn it right. The selections wrap around.
		virtual void 				browse(int dir)
		{
			auto it=selections.find(current_key);

			if(dir < 0)
			{
				if(it==selections.begin()) //Wrap around...
				{
					current_key=selections.rbegin()->first;
				}
				else
				{
					current_key=std::prev(it)->first;
				}
			}
			else
			{
				auto next=std::next(it);
				if(next==std::end(selections))
				{
					current_key=selections.begin()->first;
				}
				else
				{
					current_key=next->first;
				}
			}
		}

		//!Sets the selection by its value. 

		//!This is particularly useful if the value is known beforehand.
		void 					set_by_value(const Tvalue& value)
		{
			for(auto& s : selections)
			{
				if(s.second.value==value) 
				{
					current_key=s.first;
					return;
				}
			}

			throw options_menu_exception("value does not exist for set_by_value");
		}

		//!Sets the selection by the typename of the menu.

		//!Useful if the value is known beforehand.
		void					set_by_key(const Tkey& key_sel)
		{
			if(!selections.count(key_sel)) 
			{
				throw options_menu_exception("key does not exist for set_by_key");
			}
			
			current_key=key_sel;
		}

		//!Returns the number of selections.
		size_t				size() const {return selections.size();}

		//!Checks that there are selections. Throws an error with the message if there are none.

		//!Internal use only.
		void				check_options(const std::string& msg) const
		{
			if(!selections.size()) throw options_menu_exception(msg);
		}

		//!Inserts a new selection into the option.

		//!Selections are formed by a Tkey (menu template key), TValue
		//!(option template key) and std::string (visible name).
		void				insert(const Tkey& key, const Tvalue& v, const std::string& n)
		{
			if(!selections.size()) current_key=key;
			selections[key]={v, n};
		}

		//!Removes the selection identified by the template key.
		void 				erase(const Tkey& key_sel)
		{
			if(!selections.count(key_sel)) throw options_menu_exception("key does not exist for erase");
			selections.erase(key_sel);
		}

		//!Translates all selections with the selection struct.
	
		//!Multiple passes with multiple structs are needed to fully
		//!translate the menu.
		virtual void			translate(const translation_struct& t)
		{
			for(auto& s : selections)
				t.replace(s.first, s.second.name);
		}

		//!Constructor.
		option_menu_templated(const std::string& n):base_selection(n), current_key() {}
	};

	//!Represents an integer value within a given range.
	struct option_menu_int:public base_selection
	{
		ranged_value<int>		value; //!< Ranged value for the option.

		//!Returns the current value as a string.
		virtual std::string		get_title() const {return compat::to_string(value.get());}
		//!Returns the current value as an integer.
		int				get_value() const {return value.get();}
		//!Returns its type. Internal use.
		virtual types			get_type() const {return types::tint;}
		//!Adds or substracts from the value.
		virtual void			browse(int d){value+=d;}
		//!Translates the value. Actually, it does nothing.
		virtual void			translate(const translation_struct&){}

		//!Constructor of the option.
		option_menu_int(const std::string& n, int min, int max, int a):base_selection(n), value(min, max, a) {} 
	};

	//Represents a boolean value.
	struct option_menu_bool:public base_selection
	{
		bool				value; //!< Option value.
	
		//!Returns the value as a string.
		virtual std::string		get_title() const {return compat::to_string(value);}
		//!Returns the boolean value.
		bool				get_value() const {return value ? "true" : "false";}
		//!Returns its type. Internal use.
		virtual types			get_type() const {return types::tbool;}
		//!Ignores the parameter and just flips the value.
		virtual void			browse(int){value=!value;}
		//!Translates... does nothing.
		virtual void			translate(const translation_struct&){}

		option_menu_bool(const std::string& n, bool v):base_selection(n), value(v) {} 
	};

	//Structure to represent a string.
	struct option_menu_string:public base_selection
	{
		std::string			value; //!< String value.
	
		//!Returns the string value.
		std::string			get_value() const {return value;}
		//!Returns the string value.
		virtual std::string		get_title() const {return value;}
		//!Returns its type. Internal use.
		virtual types			get_type() const {return types::tstring;}
		//!Has no effect. Values of this type are manually set.
		virtual void			browse(int){}
		//!Has no effect.
		virtual void			translate(const translation_struct&){}
		//!Constructor.
		option_menu_string(const std::string& n, const std::string& v):base_selection(n), value(v) {} 
	};

	//Structure with no associated value.
	struct option_menu_void:public base_selection
	{
		//!Does nothing.
		void				get_value() const {}
		//!Returns an empty string.
		virtual std::string		get_title() const {return "";}
		//!Returns its type. Internal use only.
		virtual types			get_type() const {return types::tvoid;}
		//!Does nothing.
		virtual void			browse(int ){}
		//!Does nothing.
		virtual void			translate(const translation_struct&){}
		//!Constructor.
		option_menu_void(const std::string& n):base_selection(n) {} 
	};

	//!Internal use. Checks that the option with the key does not exist.
	void	check_option(const Tkey& key, const std::string& msg) const
	{
		if(!options.count(key)) throw options_menu_exception(msg);
	}

	//!Internal use. Checks that there are no duplicate keys. If succesful, inserts the key in the key vector.
	void	check_unique_key(const Tkey& key) const
	{
		if(std::find(std::begin(keys), std::end(keys), key)!=std::end(keys))
			throw options_menu_exception("duplicate key detected");

		keys.push_back(key);
	}

	//!Validates of the types are the same. Throws when they are not. Internal use.
	void	validate_type(types ot, types t, const std::string& msg) const
	{
		if(ot!=t) throw new options_menu_exception(msg);
	}

	public:

	//!Creates an integer option. 
	void		insert_int(const Tkey& key, const std::string& name, int min, int max, int val)
	{
		check_unique_key(key);
		options.insert(std::pair<Tkey, uptr_base>(key, uptr_base(new option_menu_int(name, min, max, val))));
	}

	//!Creates a bool option.
	void		insert_bool(const Tkey& key, const std::string& name, bool val)
	{
		check_unique_key(key);
		options.insert(std::pair<Tkey, uptr_base>(key, uptr_base(new option_menu_bool(name, val))));
	}

	//!Creates a string option.
	void		insert_string(const Tkey& key, const std::string& name, const std::string& val)
	{
		check_unique_key(key);
		options.insert(std::pair<Tkey, uptr_base>(key, uptr_base(new option_menu_string(name, val))));
	}

	//!Creates a void option.
	void		insert_void(const Tkey& key, const std::string& name)
	{
		check_unique_key(key);
		options.insert(std::pair<Tkey, uptr_base>(key, uptr_base(new option_menu_void(name))));
	}

	//!Erases the option with the given key.
	void		erase(const Tkey& key)
	{
		check_option(key, "key does not exist for erase");
		options.erase(key);
	}

	//Inserts a templated option.

	//!Must be called specifying its Tvalue (menu.template insert_templated<std::string>(key, "value");

	template<typename Tvalue>
	void		insert_templated(const Tkey& key, const std::string& name)
	{
		check_unique_key(key);
		options.insert(std::pair<Tkey, uptr_base>(key, uptr_base(new option_menu_templated<Tvalue>(name))));
	}

	//!Inserts a selection with key_sel, value and name for the option identified by "key".
	template<typename Tvalue>
	void		insert_templated(const Tkey& key, const Tkey& key_sel, const std::string& name, const Tvalue& value)
	{
		check_unique_key(key_sel);
		check_option(key, "key does not exist for insert template: "+name);
		auto& o=options.at(key);
		validate_type(o->get_type(), types::ttemplated, "option is not templated");
		static_cast<option_menu_templated<Tvalue> *>(o.get())->insert(key_sel, value, name);
	}

	//!Erases a selection (key_sel) from a templated option (key).
	template<typename Tvalue>
	void		erase_templated(const Tkey& key, const Tkey& key_sel)
	{
		check_option(key, "key does not exist for erase template");
		auto& o=options.at(key);
		validate_type(o->get_type(), types::ttemplated, "option is not templated");
		static_cast<option_menu_templated<Tvalue> *>(o.get())->erase(key_sel);
	}

	//!Sets the value of a templated option (key) by its known key (key_sel).
	template<typename Tvalue>
	void		set_by_key_templated(const Tkey& key, const Tkey& key_sel)
	{
		check_option(key, "key does not exist for set_by_key templated");
		auto& o=options.at(key);
		validate_type(o->get_type(), types::ttemplated, "option is not templated");
		static_cast<option_menu_templated<Tvalue> *>(o.get())->set_by_key(key_sel);
	}

	//!Sets the value of a templated option (key) by its known value.
	template<typename Tvalue>
	void		set_by_value_templated(const Tkey& key, const Tvalue& value)
	{
		check_option(key, "key does not exist for set_by_value templated");
		auto& o=options.at(key);
		validate_type(o->get_type(), types::ttemplated, "option is not templated");
		static_cast<option_menu_templated<Tvalue> *>(o.get())->set_by_value(value);
	}

	//!Gets the current value for a templated option.
	template<typename Tvalue>
	Tvalue		get_value_templated(const Tkey& key) const
	{
		check_option(key, "key does not exist for get_value templated");
		const auto& o=options.at(key);
		validate_type(o->get_type(), types::ttemplated, "option is not templated");
		return static_cast<option_menu_templated<Tvalue> *>(o.get())->get_value();
	}

	//!Gets the number of selections for a templated option.
	template<typename Tvalue>
	size_t		size_templated(const Tkey& key) const
	{
		check_option(key, "key does not exist for size templated");
		auto& o=options.at(key);
		validate_type(o->get_type(), types::ttemplated, "option is not templated");
		return static_cast<option_menu_templated<Tvalue> *>(o.get())->size();
	}

	//!Browses the option key in the direction dir.
	void		browse(const Tkey& key, int dir)
	{
		check_option(key, "key does not exist for browse");
		options.at(key)->browse(dir);
	}

	//!Specifically returns integer values from an integer option.
	int	get_int(const Tkey& key) const
	{
		check_option(key, "key does not exist for get_int");
		const auto& o=options.at(key);
		validate_type(o->get_type(), types::tint, "option is not int");
		return static_cast<option_menu_int *>(o.get())->get_value();
	}

	//!Manually sets integer values for an integer option.

	//!This function is useful to load saved values of a menu from a save file.
	void	set_int(const Tkey& key, int value)
	{
		check_option(key, "key does not exist for set_int");
		auto& o=options.at(key);
		validate_type(o->get_type(), types::tint, "option is not int");
		static_cast<option_menu_int *>(o.get())->value=value;
	}

	//!Specifically returns bool values from an bool option.
	bool	get_bool(const Tkey& key) const
	{
		check_option(key, "key does not exist for get_bool");
		const auto& o=options.at(key);
		validate_type(o->get_type(), types::tbool, "option is not bool");
		return static_cast<option_menu_bool *>(o.get())->get_value();
	}

	//!Manually sets bool values for a bool option.

	//!This function is useful to load saved values of a menu from a save file.
	void	set_bool(const Tkey& key, bool value)
	{
		check_option(key, "key does not exist for set_bool");
		auto& o=options.at(key);
		validate_type(o->get_type(), types::tbool, "option is not bool");
		static_cast<option_menu_bool *>(o.get())->value=value;
	}

	//!Specifically returns string values from a string option.
	std::string	get_string(const Tkey& key) const
	{
		check_option(key, "key does not exist for get_string");
		const auto& o=options.at(key);
		validate_type(o->get_type(), types::tstring, "option is not string");
		return static_cast<option_menu_string *>(o.get())->get_value();
	}

	//!Manually sets string values for an string option.

	//!This function is useful to load saved values of a menu from a save file.
	void	set_string(const Tkey& key, const std::string& value)
	{
		check_option(key, "key does not exist for set_string");
		auto& o=options.at(key);
		validate_type(o->get_type(), types::tstring, "option is not string");
		static_cast<option_menu_string *>(o.get())->value=value;
	}

	//!Returns the option name for the given key.
	std::string	get_name(const Tkey& key) const
	{
		check_option(key, "key does not exist for get_name");
		return options.at(key)->name;
	}

	//!Returns the string value representation for the current selection in the option.
	std::string	get_title(const Tkey& key) const
	{
		check_option(key, "key does not exist for get_title");
		const auto& o=options.at(key);
		return o->get_title();
	}

	//!Translates the full menu.
	
	//!The vector must contain all pairs of Tkey and string values for the
	//!complete menu.
	void		translate(const std::vector<translation_struct>& v)
	{
		for(const auto& t : v) 
		{
			for(auto& o : options)
			{
				t.replace(o.first, o.second->name);
				o.second->translate(t);
			}
		}
	}

	//!Returns the quantity of options in a menu.
	size_t		size() const {return options.size();}

	//!Gets a vector with the different keys used in options.
	
	//!Keys for selections are not returned. This is mostly used to draw the
	//!menu: all keys are returned and then the options can be queried.
	std::vector<Tkey>		get_keys() const 
	{
		std::vector<Tkey> res; //Cannot directly return "keys", as these contain the selections too.
		for(const auto& o : options) res.push_back(o.first);
		return res;
	}

	private:

	mutable std::vector<Tkey>				keys; //!< Stores all different Tkey keys for duplicate checking.
	std::map<Tkey, std::unique_ptr<base_selection>>		options;
};

//!Convenience function to get all names of options from a menu separated by a newline.
template<typename T>
std::string	names_from_menu(const options_menu<T>& menu)
{
	std::string res;
	for(const auto& k : menu.get_keys()) res+=menu.get_name(k)+"\n";
	res.pop_back();
	return res;
}

//!Convenience function to get all names of selections from a menu separated by a newline.
template<typename T>
std::string	titles_from_menu(const options_menu<T>& menu)
{
	std::string res;
	for(const auto& k : menu.get_keys()) res+=menu.get_title(k)+"\n";
	res.pop_back();
	return res;
}

//!Mounts a previously created menu using a dnot file.

/**The first parameter is a dnot token, which is supposed to be of vector
type. The second is the menu and the third is an empty map that will be returned 
filled with an integer for each key. This integer is meant to represent the 
translation string used in, for example, the localization class. 

The translation_map parameter is only useful when external localization tools
are used, as it will indicate a translation index for each key. If no use is
to be found for it, it can be set to null.

There is no way to set names for options and selections from the file, in fact, 
every option and names selection will be given placeholder names and must
be translated later.

The tests/menu.cpp file includes an example of the menu loaded and working
in text mode.

The structure of the dnot must be rigid:

root:[			//This is the root key, with a vector value.
	{
		m:"templated",	//A templated option
		mt:"string",	//with the internal type string
		k:"10_WINDOW",	//its key is "10_WINDOW!
		t:150,		//and it will be translated by the entry 150.
		o:[		//The selections, their keys and values follow.
			{k:"01_WINDOW_10", v: "800x500", t:151},
			{k:"01_WINDOW_20", v: "1200x750", t:152},
			{k:"01_WINDOW_30", v: "1600x1000", t:153}
		]
	},
	{
		m:"templated",  //A templated option
		mt:"bool",	//internally represented by a boolean
		k:"20_HELP",	//with this key
		t:160,		//and this translation value...
		o:[		//These are the mapped selections.
			{k:"02_HELP_10", v: true, t:161},
			{k:"02_HELP_20", v: false, t:162}
		]
	},
	//Templated options can also represent int values, not seen in this example.

	{
		m:"string"	//A string option.
		k:"22_NAME",	//This is its key.
		t:210,		//And its translation entry. Its value by default is #string_placeholder_value#.
	},
	{
		m:"int",		//An integer option
		k:"25_FILESIZE",	//With a key
		t:215,			//Translation entry
		min:"1",		//Min value (set by default)
		max:"9999"		//And max value.
	},
	{
		m:"bool",		//An bool option. By default its value is "true".
		k:"27_BACKUP",		//With a key
		t:216,			//Translation entry.
	},
	{
		m:"void"		//A void option
		k:"30_EXIT",
		t:170,
	}
]
**/

template<typename tkey, typename t_transkey>
void mount_from_dnot(
	const tools::dnot_token& root,
	options_menu<tkey>& data,
	std::map<tkey, t_transkey>* translation_map=nullptr)
{
	for(const auto& opt : root.get_vector())
	{
		const /*std::string*/ tkey k_opt=opt["k"];

		//TODO: The comment would be out of sync: no more ints.
		if(translation_map!=nullptr) {
			(*translation_map)[k_opt]=(t_transkey)opt["t"]; //So here we write the key with its translation data.
		}
		
		const std::string menu_type=opt["m"];

		if(menu_type=="templated")
		{
			//Bufff... Ese "template" está para que el compilador no se grille: es el "template disambiguator", 
			//que ayuda a saber que es un método templatizado y no una propiedad seguida de "menor que".

			const std::string mt=opt["mt"];
			if(mt=="string") {
				data.template insert_templated<std::string>(k_opt, "#string_templated_placeholder#");
			}
			else if(mt=="int") {
				data.template insert_templated<int>(k_opt, "#int_templated_placeholder#");
			}
			else if(mt=="bool") {
				data.template insert_templated<bool>(k_opt, "#bool_templated_placeholder#");
			}
			else throw std::runtime_error("unknown template type "+mt);

			const auto& selections=opt["o"].get_vector();
			for(const auto& sel : selections)
			{
				const std::string k_sel=sel["k"];
				if(translation_map!=nullptr) {
					(*translation_map)[k_sel]=(t_transkey)sel["t"];
				}

				if(mt=="string") data.template insert_templated<std::string>(k_opt, k_sel, "#string_selection_placeholder#", sel["v"]);
				else if(mt=="int") data.template insert_templated<int>(k_opt, k_sel, "#int_selection_placeholder", sel["v"]); 
				else if(mt=="bool") data.template insert_templated<bool>(k_opt, k_sel, "#bool_selection_placeholder", sel["v"]);
			}
		}
		else if(menu_type=="int")
		{
			data.insert_int(k_opt, "#int_placeholder#", opt["min"], opt["max"], opt["min"]);
		}
		else if(menu_type=="bool")
		{
			data.insert_bool(k_opt, "#bool_placeholder#", true);
		}
		else if(menu_type=="string")
		{
			data.insert_string(k_opt, "#string_placeholder#", "#string_placeholder_value#");
		}
		else if(menu_type=="void")
		{
			data.insert_void(k_opt, "#void_placeholder#");
		}
	}
}

}

#endif
