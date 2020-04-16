#pragma once

#include "ranged_value.h"
#include "compatibility_patches.h"

#include <rapidjson/document.h>

#include <iostream>
#include <type_traits>
#include <string>
#include <map>
#include <vector>
#include <exception>
#include <algorithm>
#include <memory>

namespace tools {

//!Exception thrown by the options_menu class.
class options_menu_exception:
	public std::runtime_error {
	public:
	//!Class constructor.
	options_menu_exception(const std::string& s):std::runtime_error(s) {}
};

//!Data representation of a single depth menu.
/**
This class is built to represent a single depth menu with N options. Options 
might have a free typed value (such a string, boolean, integer or even no 
type) or a value in a range of N fixed options:

menu:
	language: [English, Spanish, Japanese]
	lives: [1-99]
	timeout: [60]
	name: [something]
	fullscreen: [true]
	back 

The first level, "menu" is the class itself. Each node (screen_resolution, 
sound_volume, language and lives) is a "entry" and for each option, different
"values" are given. 

Entries and values are identified by an unique key. This key must be 
completely unique: if shared between, for example, an entry and a value,
an exception will be thrown. The key is the typename of the template.

The order of entries and values is determined by the natural order of the
typename. For the best flexibility, std::strings are recommended.

As stated,there are different types of entries:

	int: used to represent integer values within a range.
	bool: used to represent true or false
	string: used to store strings.
	void: does not store anything, like the access to a submenu.
	choice: used to represent a choice of any given type (string, int, bool...) among many.

Each entry has a name. This name is translatable with the "translate" method.

Menus can be built by code or from a JSON node using the rapidjson library.

Finally, there is a particular exception "options_menu_exception" thrown for
every error.
*/

template<typename Tkey>
class options_menu {
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

	//!Defines the different types of choices.
	enum class types {ttemplated, tint, tbool, tstring, tvoid};

	//!Base class for all entries.
	struct base_entry {

		std::string 			name; //!< All entries have a name string.

		//!Will return the name of the current entry (or its equivalent).
		virtual	std::string		get_title() const=0;
		//!Returns its type. Internal use.
		virtual types			get_type() const=0;
		//!Chooses the next/previous value in the option.
		virtual void			browse(int)=0;
		//!Translates the entry. 
		virtual void			translate(const translation_struct& t)=0;
		//Base constructor.
						base_entry(const std::string& n):name(n){
		}
	};

	
	typedef std::unique_ptr<base_entry>	uptr_base;

	//!Structure to represent any type with a string. 

	//!Consists of N choices (of type Tvalue), each one represented by a
	//!different string. For example, the values 0,1 and 2 (int) could 
	//!repreent "txt", "html" and "latex" output. It is implemented in 
	//!terms of a std::map.
	template<typename Tvalue>
	struct entry_choice:public base_entry {
		//!Pair of value and name.
		struct choice {
			Tvalue			value;	//!< Stored value.
			std::string		name; 	//!< Display name.
		};

		std::map<Tkey, choice>	choices; //!< Internal option map.
		Tkey				current_key;	//!< Currently selected key.

		//!Returns the typename value for the current selection (0,1,2 in the example).
		Tvalue				get_value() const {
			check_options("option with no choices for get_value");
			return choices.at(current_key).value;
		}

		//!Returns the string that represents the current selection ("txt", "html" or "latex" in the example).
		virtual std::string		get_title() const {
			check_options("option with no choices for get_title");
			return choices.at(current_key).name;
		}

		//!Returns the type of this class.
		virtual types				get_type() const {
			return types::ttemplated;
		}

		//!Changes the current selection. 

		//!Values smaller than zero turn the selection "left" and larger
		//!than zero turn it right. The choices wrap around.
		virtual void 				browse(int dir) {
			auto it=choices.find(current_key);

			if(dir < 0) {
				if(it==choices.begin()) {
					current_key=choices.rbegin()->first;
				}
				else {
					current_key=std::prev(it)->first;
				}
			}
			else {
				auto next=std::next(it);
				if(next==std::end(choices)) {
					current_key=choices.begin()->first;
				}
				else {
					current_key=next->first;
				}
			}
		}

		//!Sets the selection by its value. 

		//!This is particularly useful if the value is known beforehand.
		void 					set_by_value(const Tvalue& value) {

			for(auto& s : choices) {
				if(s.second.value==value) {
					current_key=s.first;
					return;
				}
			}

			throw options_menu_exception("value does not exist for set_by_value");
		}

		//!Sets the selection by the typename of the menu.

		//!Useful if the value is known beforehand.
		void					set_by_key(const Tkey& key_sel) {

			if(!choices.count(key_sel)) {
				throw options_menu_exception("key does not exist for set_by_key");
			}
			
			current_key=key_sel;
		}

		//!Returns the number of choices.
		size_t				size() const {
			return choices.size();
		}

		//!Checks that there are choices. Throws an error with the message if there are none.

		//!Internal use only.
		void				check_options(const std::string& msg) const {
			if(!choices.size()) {
				throw options_menu_exception(msg);
			}
		}

		//!Inserts a new selection into the option.

		//!choices are formed by a Tkey (menu template key), TValue
		//!(option template key) and std::string (visible name).
		void				insert(const Tkey& key, const Tvalue& v, const std::string& n) {
			if(!choices.size()) {
				current_key=key;
			}
			choices[key]={v, n};
		}

		//!Removes the selection identified by the template key.
		void 				erase(const Tkey& key_sel) {

			if(!choices.count(key_sel)) {
				throw options_menu_exception("key does not exist for erase");
			}

			choices.erase(key_sel);
		}

		//!Translates all choices with the selection struct.

		//!Multiple passes with multiple structs are needed to fully
		//!translate the menu.
		virtual void			translate(const translation_struct& t) {

			for(auto& s : choices) {
				t.replace(s.first, s.second.name);
			}
		}

		//!Constructor.
		entry_choice(const std::string& n)
			:base_entry(n), current_key() {
		}
	};

	//!Represents an integer value within a given range.
	struct entry_int:public base_entry {

		ranged_value<int>		value; //!< Ranged value for the option.

		//!Returns the current value as a string.
		//TODO: Fuck the compat thing xD!.
		virtual std::string		get_title() const {return compat::to_string(value.get());}
		//!Returns the current value as an integer.
		int						get_value() const {return value.get();}
		//!Returns its type. Internal use.
		virtual types			get_type() const {return types::tint;}
		//!Adds or substracts from the value.
		virtual void			browse(int d){value+=d;}
		//!Translates the value. Actually, it does nothing.
		virtual void			translate(const translation_struct&){}

		//!Constructor of the option.
		entry_int(const std::string& n, int min, int max, int a)
			:base_entry(n), value(min, max, a) {
		} 
	};

	//Represents a boolean value.
	struct entry_bool:public base_entry {
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

		entry_bool(const std::string& n, bool v)
			:base_entry(n), value(v) {
		} 
	};

	//Structure to represent a string.
	struct entry_string:public base_entry {
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
		entry_string(const std::string& n, const std::string& v)
			:base_entry(n), value(v) {
		} 
	};

	//Structure with no associated value.
	struct entry_void:public base_entry {
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
		entry_void(const std::string& n):base_entry(n) {} 
	};

	//!Internal use. Checks that the option with the key does not exist.
	void	check_option(const Tkey& key, const std::string& msg) const {

		if(!options.count(key)) throw options_menu_exception(msg);
	}

	//!Internal use. Checks that there are no duplicate keys. If succesful, inserts the key in the key vector.
	void	check_unique_key(const Tkey& key) const {

		if(std::find(std::begin(keys), std::end(keys), key)!=std::end(keys)) {
			throw options_menu_exception("duplicate key detected");
		}

		keys.push_back(key);
	}

	//!Validates of the types are the same. Throws when they are not. Internal use.
	void	validate_type(types ot, types t, const std::string& msg) const {

		if(ot!=t) throw new options_menu_exception(msg);
	}

	public:

	//!Creates an integer option. 
	void		insert_int(const Tkey& key, const std::string& name, int min, int max, int val) {

		check_unique_key(key);
		options.insert(
			std::pair<Tkey, uptr_base>(
				key, 
				uptr_base(new entry_int(name, min, max, val))
			)
		);
	}

	//!Creates a bool option.
	void		insert_bool(const Tkey& key, const std::string& name, bool val) {

		check_unique_key(key);
		options.insert(
			std::pair<Tkey, uptr_base>(
				key, 
				uptr_base(new entry_bool(name, val))
			)
		);
	}

	//!Creates a string option.
	void		insert_string(const Tkey& key, const std::string& name, const std::string& val) {

		check_unique_key(key);
		options.insert(
			std::pair<Tkey, uptr_base>(
				key, 
				uptr_base(new entry_string(name, val))
			)
		);
	}

	//!Creates a void option.
	void		insert_void(const Tkey& key, const std::string& name) {

		check_unique_key(key);
		options.insert(
			std::pair<Tkey, uptr_base>(
				key, 
				uptr_base(new entry_void(name))
			)
		);
	}

	//!Erases the option with the given key.
	void		erase(const Tkey& key) {

		check_option(key, "key does not exist for erase");
		options.erase(key);
	}

	//Inserts a choice option.

	//!Must be called specifying its Tvalue (menu.template insert_choice<std::string>(key, "value");
	template<typename Tvalue>
	void		insert_choice(const Tkey& key, const std::string& name) {

		check_unique_key(key);
		options.insert(
			std::pair<Tkey, uptr_base>(
				key, 
				uptr_base(new entry_choice<Tvalue>(name))
			)
		);
	}

	//!Inserts a selection with key_sel, value and name for the option identified by "key".
	template<typename Tvalue>
	void		insert_choice(const Tkey& key, const Tkey& key_sel, const std::string& name, const Tvalue& value) {

		check_unique_key(key_sel);
		check_option(key, "key does not exist for insert choice: "+name);
		auto& o=options.at(key);
		validate_type(o->get_type(), types::ttemplated, "option is not choice");
		static_cast<entry_choice<Tvalue> *>(o.get())->insert(key_sel, value, name);
	}

	//!Erases a value (key_sel) from a choice option (key).
	template<typename Tvalue>
	void		erase_choice(const Tkey& key, const Tkey& key_sel) {

		check_option(key, "key does not exist for erase choice");
		auto& o=options.at(key);
		validate_type(o->get_type(), types::ttemplated, "option is not choice");
		static_cast<entry_choice<Tvalue> *>(o.get())->erase(key_sel);
	}

	//!Sets the value of a choice option (key) by its known key (key_sel).
	template<typename Tvalue>
	void		set_by_key_choice(const Tkey& key, const Tkey& key_sel) {

		check_option(key, "key does not exist for set_by_key choice");
		auto& o=options.at(key);
		validate_type(o->get_type(), types::ttemplated, "option is not choice");
		static_cast<entry_choice<Tvalue> *>(o.get())->set_by_key(key_sel);
	}

	//!Sets the value of a choice option (key) by its known value.
	template<typename Tvalue>
	void		set_by_value_choice(const Tkey& key, const Tvalue& value) {

		check_option(key, "key does not exist for set_by_value choice");
		auto& o=options.at(key);
		validate_type(o->get_type(), types::ttemplated, "option is not choice");
		static_cast<entry_choice<Tvalue> *>(o.get())->set_by_value(value);
	}

	//!Gets the current value for a templated option.
	template<typename Tvalue>
	Tvalue		get_value_choice(const Tkey& key) const {

		check_option(key, "key does not exist for get_value choice");
		const auto& o=options.at(key);
		validate_type(o->get_type(), types::ttemplated, "option is not choice");
		return static_cast<entry_choice<Tvalue> *>(o.get())->get_value();
	}

	//!Gets the number of choices for a templated option.
	template<typename Tvalue>
	size_t		size_choice(const Tkey& key) const {

		check_option(key, "key does not exist for size choice");
		auto& o=options.at(key);
		validate_type(o->get_type(), types::ttemplated, "option is not choice");
		return static_cast<entry_choice<Tvalue> *>(o.get())->size();
	}

	//!Browses the option key in the direction dir.
	void		browse(const Tkey& key, int dir) {

		check_option(key, "key does not exist for browse");
		options.at(key)->browse(dir);
	}

	//!Specifically returns integer values from an integer option.
	int	get_int(const Tkey& key) const {

		check_option(key, "key does not exist for get_int");
		const auto& o=options.at(key);
		validate_type(o->get_type(), types::tint, "option is not int");
		return static_cast<entry_int *>(o.get())->get_value();
	}

	//!Manually sets integer values for an integer option.

	//!This function is useful to load saved values of a menu from a save file.
	void	set_int(const Tkey& key, int value) {

		check_option(key, "key does not exist for set_int");
		auto& o=options.at(key);
		validate_type(o->get_type(), types::tint, "option is not int");
		static_cast<entry_int *>(o.get())->value=value;
	}

	//!Specifically returns bool values from an bool option.
	bool	get_bool(const Tkey& key) const {

		check_option(key, "key does not exist for get_bool");
		const auto& o=options.at(key);
		validate_type(o->get_type(), types::tbool, "option is not bool");
		return static_cast<entry_bool *>(o.get())->get_value();
	}

	//!Manually sets bool values for a bool option.

	//!This function is useful to load saved values of a menu from a save file.
	void	set_bool(const Tkey& key, bool value) {

		check_option(key, "key does not exist for set_bool");
		auto& o=options.at(key);
		validate_type(o->get_type(), types::tbool, "option is not bool");
		static_cast<entry_bool *>(o.get())->value=value;
	}

	//!Specifically returns string values from a string option.
	std::string	get_string(const Tkey& key) const {

		check_option(key, "key does not exist for get_string");
		const auto& o=options.at(key);
		validate_type(o->get_type(), types::tstring, "option is not string");
		return static_cast<entry_string *>(o.get())->get_value();
	}

	//!Manually sets string values for an string option.

	//!This function is useful to load saved values of a menu from a save file.
	void	set_string(const Tkey& key, const std::string& value) {

		check_option(key, "key does not exist for set_string");
		auto& o=options.at(key);
		validate_type(o->get_type(), types::tstring, "option is not string");
		static_cast<entry_string *>(o.get())->value=value;
	}

	//!Returns the option name for the given key.
	std::string	get_name(const Tkey& key) const {

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
	void		translate(const std::vector<translation_struct>& v) {

		for(const auto& t : v) {
			for(auto& o : options) {
				t.replace(o.first, o.second->name);
				o.second->translate(t);
			}
		}
	}

	//!Returns the quantity of entries in a menu.
	size_t		size() const {

		return options.size();
	}

	//!Gets a vector with the different keys used in entries.
	
	//!Keys for choices are not returned. This is mostly used to draw the
	//!menu: all keys are returned and then the options can be queried.
	std::vector<Tkey>		get_keys() const {

		std::vector<Tkey> res; //Cannot directly return "keys", as these contain the choices too.
		for(const auto& o : options) {
			res.push_back(o.first);
		}
		return res;
	}

	private:

	mutable std::vector<Tkey>				keys; //!< Stores all different Tkey keys for duplicate checking.
	std::map<Tkey, std::unique_ptr<base_entry>>		options;
};

//!Convenience function to get all names of options from a menu separated by a newline.
template<typename T>
std::string	names_from_menu(const options_menu<T>& menu) {

	std::string res;
	for(const auto& k : menu.get_keys()) {
		res+=menu.get_name(k)+"\n";
	}
	res.pop_back();
	return res;
}

//!Convenience function to get all names of choices from a menu separated by a newline.
template<typename T>
std::string	titles_from_menu(const options_menu<T>& menu) {

	std::string res;
	for(const auto& k : menu.get_keys()) {
		res+=menu.get_title(k)+"\n";
	}
	res.pop_back();
	return res;
}

//!Mounts a previously created menu using a JSON value with rapidjson..

/**The first parameter is a rapidJson value, which is supposed to be of array
type. The second is the menu and the third is an empty map that will be returned 
filled with an integer for each key. This integer is meant to represent the 
translation string used in, for example, the localization class. 

The translation_map parameter is only useful when external localization tools
are used, as it will indicate a translation index for each key. If no use is
to be found for it, it can be set to nullptr.

There is no way to set names for options and choices from the file, in fact, 
every option and names selection will be given placeholder names and must
be translated later. This is so by design.

The examples/menu code includes an example of the menu loaded and working
in text mode.

The structure of the json must be rigid, lest this function will throw.

[			//This is the root key, with an array value.
	{		//An object.
		"type":"choice",	//A templated option
		"key":"10_WINDOW",	//its key is "10_WINDOW!
		"translation":150,		//and it will be translated by the entry 150.
		"properties" : {
			"value_type":"string",	//with the internal type string
			"values":[		//The choices, their keys and values follow.
				{"key":"01_WINDOW_10", "value": "800x500", "translation":151},
				{"key":"01_WINDOW_20", "value": "1200x750", "translation":152},
				{"key":"01_WINDOW_30", "value": "1600x1000", "translation":153}
			]
		}
	},
	{
		"type":"choice",  //A templated option
		"key":"20_HELP",	//with this key
		"translation":160,		//and this translation value...
		"properties":{
			"value_type":"bool",	//internally represented by a boolean
			"values":[		//These are the mapped choices.
				{"key":"02_HELP_10", "value": true, "translation":161},
				{"key":"02_HELP_20", "value": false, "translation":162}
			]
		}
	},
	//Choice options can also represent int values, not seen in this example.

	{
		"type":"string",	//A string option.
		"key":"22_NAME",	//This is its key.
		"translation":210		//And its translation entry. Its value by default is #string_placeholder_value#.
	},
	{
		"type":"int",		//An integer option
		"key":"25_FILESIZE",	//With a key
		"translation":215,			//Translation entry
		"properties": {
			"min": 1,		//Min value (set by default)
			"max": 9999		//And max value.
		}
	},
	{
		"type":"bool",		//An bool option. By default its value is "true".
		"key":"27_BACKUP",		//With a key
		"translation":216			//Translation entry.
	},
	{
		"type":"void",		//A void option
		"key":"30_EXIT",
		"translation":170
	}
]
**/

template<typename tkey, typename t_transkey>
void options_menu_from_json(
	const rapidjson::Value& _root,
	options_menu<tkey>& _target_menu,
	std::map<tkey, t_transkey>* _translation_map=nullptr)
{
	const char *	k_key="key",
					k_translation="translation",
					k_type="type",
					k_properties="properties",
					k_min="min",
					k_max="max",
					k_value_type="value_type",
					k_values="values";

	if(!_root.IsObject()) {
		throw std::runtime_error("root node must be an object");
	}

	for(const auto& val : _root.GetArray()) {

		if(!val.IsObject()) {
			throw std::runtime_error("entry must be an object");
		}

		if(!val.HasMember(k_key)) {
			throw std::runtime_error("entry node must have a key value");
		}

		//TODO: Not really, must be of type T... Which would be fun to do.
		//if(!val[k_key].IsString()) {
		//	throw std::runtime_error("the key value must be an string");
		//}

		if(!val.HasMember(k_translation)) {
			throw std::runtime_error("entry node must have a translation value");
		}

		//Nooope.
		if(!val[k_translation].IsInt()) {
			throw std::runtime_error("the translation value must be an integer");
		}

		if(!val.HasMember(k_type)) {
			throw std::runtime_error("entry node must have a type value");
		}

		if(!val[k_type].IsString()) {
			throw std::runtime_error("the type value must be an string");
		}

		const tkey k_choice=opt[k_key];
		if(_translation_map!=nullptr) {
			(*_translation_map)[k_choice]=(t_transkey)opt[k_translation]; //So here we write the key with its translation data.
		}
		
		const std::string choice_type=opt[k_type].GetString();

		if(choice_type=="choice") {

			if(!val.HasMember(k_properties)) {
				throw std::runtime_error("choice entry must have a properties node");
			}

			const auto& properties=opt[k_properties];
			if(!properties.IsObject()) {
				throw std::runtime_error("the properties node must be an object");
			}

			if(!properties.HasMember(k_value_type)) {
				throw std::runtime_error("the properties node must have a value_type property");
			}

			if(!properties[k_value_type].isString()) {
				throw std::runtime_error("value_type must be a string");
			}

			if(!properties.HasMember(k_values)) {
				throw std::runtime_error("the properties node must have a values property");
			}

			if(!properties[k_values].IsArray()) {
				throw std::runtime_error("the values property must be an array");
			}

			//Bufff... Ese "template" está para que el compilador no se grille: es el "template disambiguator", 
			//que ayuda a saber que es un método templatizado y no una propiedad seguida de "menor que".
			const std::string value_type=properties[k_value_type];
			if(value_type=="string") {
				_target_menu.template insert_choice<std::string>(k_choice, "#string_choice_placeholder#");
			}
			else if(value_type=="int") {
				_target_menu.template insert_choice<int>(k_choice, "#int_choice_placeholder#");
			}
			else if(value_type=="bool") {
				_target_menu.template insert_choice<bool>(k_choice, "#bool_choice_placeholder#");
			}
			else {
				throw std::runtime_error("unknown choice type "+mt);
			}

			for(const auto& choice : properties[k_values].GetArray()) {

				if(!choice.IsObject()) {
					throw std::runtime_error("each choice must be an object");
				}

				if(!choice.HasMember(k_key)) {
					throw std::runtime_error("choice must have a key property");
				}

				//TODO:NOOOPE check k_key Must be of type K
				//if(!choice[k_key].IsString()) {
				//	throw std::runtime_error("choice key must be a string");
				//}

				if(!choice.HasMember(k_value)) {
					throw std::runtime_error("choice must have a value property");
				}

				if(!choice.HasMember(k_translation)) {
					throw std::runtime_error("choice must have a translation property");
				}

				///TODO: Again, noooope.
				//if(!choice[k_translation].IsInt()) {
				//	throw std::runtime_error("choice translation property must be an integer");
				//}

				const std::string k_val=sel[k_key];
				if(_translation_map!=nullptr) {
					(*_translation_map)[k_val]=(t_transkey)sel[k_translation];
				}

				if(value_type=="string") {
					_target_menu.template insert_choice<std::string>(k_choice, k_val.GetString(), "#string_value_placeholder#", choice["value"].GetString());
				}
				else if(value_type=="int") {
					_target_menu.template insert_choice<int>(k_choice, k_val.GetInt(), "#int_value_placeholder", choice["value"].GetInt()); 
				}
				else if(value_type=="bool") {
					_target_menu.template insert_choice<bool>(k_choice, k_val.GetBool(), "#bool_value_placeholder", choice["value"].GetBool());
				}
			}
		}
		else if(choice_type=="int") {

			if(!val.HasMember(k_properties)) {
				throw std::runtime_error("int entry must have a properties node");
			}

			const auto& properties=opt[k_properties];
			if(!properties.IsObject()) {
				throw std::runtime_error("the properties node must be an object");
			}

			if(!properties.HasMember(k_min)) {
				throw std::runtime_error("the properties node must have a min property");
			}

			if(!properties[k_min].IsInt()) {
				throw std::runtime_error("min property must be an integer");
			}

			if(!properties.HasMember(k_max)) {
				throw std::runtime_error("the properties node must have a min property");
			}

			if(!properties[k_max].IsInt()) {
				throw std::runtime_error("max property must be an integer");
			}

			int min=properties[k_min].GetInt(),
				max=properties[k_max].GetInt();

			_target_menu.insert_int(k_choice, "#int_placeholder#", min, max, min);
		}
		else if(choice_type=="bool") {

			_target_menu.insert_bool(k_choice, "#bool_placeholder#", true);
		}
		else if(choice_type=="string") {

			_target_menu.insert_string(k_choice, "#string_placeholder#", "#string_placeholder_value#");
		}
		else if(choice_type=="void") {
			_target_menu.insert_void(k_choice, "#void_placeholder#");
		}
		else {
			throw std::runtime_error("unknown choice type "+choice_type);
		}
	}
}

}

