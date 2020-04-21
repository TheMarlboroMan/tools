#pragma once

#include "ranged_value.h"
#include "json.h"

#include <rapidjson/document.h>

#include <iostream>
#include <type_traits>
#include <string>
#include <map>
#include <vector>
#include <exception>
#include <algorithm>
#include <memory>
#include <sstream>

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
This class is built to represent a single depth menu with N entries. Entries
might have a free typed value (such a string, boolean, integer or even no
type) or a value in a range of N fixed values:

menu:
	language: [English, Spanish, Japanese]
	lives: [1-99]
	timeout: [60]
	name: [something]
	fullscreen: [true]
	back

The first level, "menu" is the class itself. Each node (screen_resolution,
sound_volume, language and lives) is a "entry" and for each "entry", different
"values" may be given, so:

menu:
	entry
	entry
		value
		value
		value
	entry...

Entries and values are identified by an unique key. This key must be
completely unique: if shared between, for example, an entry and a value,
an exception will be thrown. The key is the typename of the template. The key
must be printable by streams.

The order of entries and values is determined by the natural order of the
typename. For the best flexibility, std::strings are recommended.

As stated,there are different types of entries:

	int: used to represent integer values within a range.
	bool: used to represent true or false
	string: used to store strings.
	void: does not store anything, like the access to a submenu.
	choice: used to represent a choice of any given type (string, int, bool...) among many.

Each entry has a name.

Menus can be built by code or from a JSON node using the rapidjson library.

Finally, there is a particular exception "options_menu_exception" thrown for
every error.
*/

template<typename Tkey>
class options_menu {

	public:

	enum class browse_dir{next, previous};

	private:

	//Compile time assertion that a menu will be created with valid key types.
	template<typename t_keytype>
	void is_possible_key() const {
		static_assert(
			std::is_same<int, t_keytype>::value
			|| std::is_same<std::string, t_keytype>::value,
//			|| std::is_same<const char *, t_keytype>::value,
			"only integer and string types are permitted for choices"
		);
	}


	//Compile time assertion that a choices will be created with valid value types.
	template<typename Tvalue>
	void is_possible_value() const  {

		static_assert(
			std::is_same<bool, Tvalue>::value
			|| std::is_same<int, Tvalue>::value
			|| std::is_same<std::string, Tvalue>::value
			|| std::is_same<const char *, Tvalue>::value,
			"only integer, string and boolean types are permitted for choices"
		);
	}

	//!Defines the different types of choices.
	enum class types {tchoice, tint, tbool, tstring, tvoid};

	//!Assignment overloads for the "set" function, so we can work around
	//templates.
	void	assign(const Tkey& _key, int _value) {

		auto& o=entries.at(_key);
		static_cast<entry_int *>(o.get())->value=_value;
	}

	void	assign(const Tkey& _key, bool _value) {

		auto& o=entries.at(_key);
		static_cast<entry_bool *>(o.get())->value=_value;
	}

	void	assign(const Tkey& _key, const std::string& _value) {

		auto& o=entries.at(_key);
		static_cast<entry_string *>(o.get())->value=_value;
	}

	void	assign(const Tkey& _key, const char * _value) {

		auto& o=entries.at(_key);
		static_cast<entry_string *>(o.get())->value=std::string{_value};
	}

	template<typename tvalue>
	void	assign(const Tkey& _key, tvalue _value, types _value_type) {

		auto fail=[_key](const std::string& _msg) {
			std::stringstream ss;
			ss<<_msg<<" for key '"<<_key<<"'";
			throw options_menu_exception(ss.str());
		};

		switch(_value_type) {
			case types::tint:
				if(! std::is_same<int, tvalue>::value) {
					fail("type must be an integer for choice");
				}
				static_cast<entry_choice<tvalue>*>(entries.at(_key).get())->set(_value);
			break;
			case types::tbool:
				if(! std::is_same<bool, tvalue>::value) {
					fail("type must be a boolean for choice");
				}
				static_cast<entry_choice<tvalue>*>(entries.at(_key).get())->set(_value);
			break;
			case types::tstring:
				if(
					! std::is_same<std::string, tvalue>::value
					&& !std::is_same<const char *, tvalue>::value
				) {
					fail("type must be a string for choice");
				}
				static_cast<entry_choice<tvalue>*>(entries.at(_key).get())->set(_value);
			break;
			case types::tvoid:
			case types::tchoice:
				fail("invalid choice value to assign, must be integer, bool or string");
			break;
		}
	}

	//!Returns a printable representation of the possible types for key and
	//!choices, for debug purposes.
	const char * translate_type(types _t) {
		switch(_t) {
			case types::tchoice:	return "choice";
			case types::tint:		return "int";
			case types::tbool:		return "bool";
			case types::tstring:	return "string";
			case types::tvoid:		return "void";
		}
		return "";
	}

	//!Base class for all entries.
	struct base_entry {

		std::string 			name; //!< All entries have a name string. Will end up being the printable representation of it, like "window size".

		//!Will return a printable value for the current entry (or its equivalent).
		virtual	std::string		get_str_value() const=0;

		//!Returns its type. Internal use.
		virtual types			get_type() const=0;

		//!Returns its value type. Internal use. Its only use is for choice types.
		virtual types			get_value_type() const {return get_type();}

		//!Chooses the next/previous value in the option.
		virtual void			browse(browse_dir)=0;

		//Base constructor.
								base_entry(const std::string& n):name(n){
		}
	};


	using uptr_base=std::unique_ptr<base_entry>;

	//!Structure to represent any type with a string.

	//!Consists of N choices (of type Tvalue), each one represented by a
	//!different string. For example, the values 0,1 and 2 (int) could
	//!represent "txt", "html" and "latex" output. It is implemented in
	//!terms of a std::map.
	template<typename Tvalue>
	struct entry_choice:public base_entry {

		types				value_type; //The type of the value (TValue) must be stored in order to be retrieved.

		//!Pair of value and name.
		struct choice {
			Tvalue			value;	//!< Stored value.
			std::string		str_value; 	//!< Display name.
		};

		std::vector<choice>	choices; //!< Internal option map.
		std::size_t			current_index=0;	//!< Currently selected key.

		//!Returns the typename value for the current selection (0,1,2 in the example).
		Tvalue				get_value() const {

			if(!choices.size()) {
				throw options_menu_exception("choice with no values for get_value");
			}

			return choices.at(current_index).value;
		}

		//!Returns the string that represents the current selection ("txt", "html" or "latex" in the example).
		virtual std::string		get_str_value() const {

			if(!choices.size()) {
				throw options_menu_exception("choice with no values for get_str_value");
			}

			return choices.at(current_index).str_value;
		}

		//!Returns the type of this class.
		virtual types				get_type() const {
			return types::tchoice;
		}

		types                       get_value_type() const {
			return value_type;
		}

		//!Changes the current selection.

		//!The choices wrap around.
		virtual void 				browse(browse_dir _dir) {

			if(_dir==browse_dir::previous) {

				--current_index;
				if(current_index < 0) {
					current_index=choices.size()-1;
				}
			}
			else {
				++current_index;
				if(current_index == choices.size()) {
					current_index=0;
				}
			}
		}

		//!Internal value comparison functions.

		//This is actually fun: if we do "set" with a const char *
		//the template code will try to call == on it, which will not do what
		//we want. However, we can force type conversion with these helpers.
		bool                    compare_values(bool _a, bool _b) const {return _a==_b;}
		bool                    compare_values(int _a, int _b) const {return _a==_b;}
		bool                    compare_values(const std::string& _a, const std::string _b) const {return _a==_b;}

		//!Sets the selection by its value.
		void 					set(const Tvalue& _value) {

			auto it=std::find_if(
				std::begin(choices),
				std::end(choices),
				[&_value, this](const choice& _choice) {return compare_values(_value, _choice.value);}
			);

			if(it==std::end(choices)) {
				std::stringstream ss;
				ss<<"value does not exist for set '"<<_value<<"'";
				throw options_menu_exception(ss.str());
			}

			current_index=std::distance(std::begin(choices), it);
		}

		//!Returns the number of choices.
		size_t				size() const {
			return choices.size();
		}

		//!Inserts a new selection into the option.

		//!choices are formed by a Tkey (menu template key), TValue
		//!(option template key) and std::string (visible name).
		//TODO: What's with the key parameter? Will we keep that???
		void				insert(const Tvalue& _value, const std::string& _name) {

			//TODO: Values should be unique!!!!!!
			//TODO: WFT IS NAME???
std::cout<<"value: "<<_value<<" name: "<<_name<<std::endl;

			choices.push_back({_value, _name});
		}

		//!Removes the selection identified by the given value.
		//TODO: Identify by value...
		/*
		void 				erase(const Tkey& key_sel) {

			if(!choices.count(key_sel)) {
				throw options_menu_exception("key does not exist for erase");
			}

			choices.erase(key_sel);
		}
		*/

		//!Constructor.
		entry_choice(const std::string& _name)
			:base_entry(_name), value_type{types::tvoid} {

			if(std::is_same<int, Tvalue>::value) {
				value_type=types::tint;
			}
			else if(std::is_same<bool, Tvalue>::value) {
				value_type=types::tbool;
			}
			else if(std::is_same<const char *, Tvalue>::value) {
				value_type=types::tstring;
			}
			else if(std::is_same<std::string, Tvalue>::value) {
				value_type=types::tstring;
			}
		}
	};

	//!Represents an integer value within a given range.
	struct entry_int:public base_entry {

		ranged_value<int>		value; //!< Ranged value for the option.

		//!Returns the current value as a string.
		virtual std::string		get_str_value() const {return std::to_string(value.get());}
		//!Returns the current value as an integer.
		int						get_value() const {return value.get();}
		//!Returns its type. Internal use.
		virtual types			get_type() const {return types::tint;}
		//!Adds or substracts from the value.
		virtual void			browse(browse_dir _dir){
			value+=_dir==browse_dir::next ? 1 : -1;
		}

		//!Constructor of the option.
		entry_int(const std::string& n, int min, int max, int a)
			:base_entry(n), value(min, max, a) {
		}
	};

	//Represents a boolean value.
	struct entry_bool:public base_entry {
		bool				value; //!< Option value.

		//!Returns the value as a string.
		virtual std::string		get_str_value() const {return std::to_string(value);}

		//!Returns the boolean value.
		bool				get_value() const {return value ? "true" : "false";}

		//!Returns its type. Internal use.
		virtual types			get_type() const {return types::tbool;}

		//!Ignores the parameter and just flips the value.
		virtual void			browse(browse_dir){value=!value;}

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
		virtual std::string		get_str_value() const {return value;}

		//!Returns its type. Internal use.
		virtual types			get_type() const {return types::tstring;}

		//!Has no effect. Values of this type are manually set.
		virtual void			browse(browse_dir){}

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
		virtual std::string		get_str_value() const {return "";}

		//!Returns its type. Internal use only.
		virtual types			get_type() const {return types::tvoid;}

		//!Does nothing.
		virtual void			browse(browse_dir ){}

		//!Constructor.
		entry_void(const std::string& n):base_entry(n) {}
	};

	//!Internal use. Checks that the option with the key does not exist.
	void	check_entry(const Tkey& key, const std::string& msg) const {

		if(!entries.count(key)) {
			throw options_menu_exception(msg);
		}
	}

	//!Internal use. Checks that there are no duplicate keys. If succesful, inserts the key in the key vector.
	void	check_unique_key(const Tkey& key) const {

		if(std::find(std::begin(keys), std::end(keys), key)!=std::end(keys)) {

			std::stringstream ss;
			ss<<"duplicate key detected: '"<<key<<"'";
			throw options_menu_exception(ss.str());
		}

		keys.push_back(key);
	}

	//TODO: Don't really see the use...
	//!Validate that types are the same. Throws when they are not. Internal use.
	void		check_type(types ot, types t, const Tkey& _key, const std::string& msg) const {

		if(ot!=t) {
			std::stringstream ss;
			ss<<msg<<" for key '"<<_key<<"'";
			throw options_menu_exception(ss.str());
		}
	}

	public:

	//!Erases the entry with the given key.
	void		erase(const Tkey& key) {

		is_possible_key<Tkey>();
		check_entry(key, "key does not exist for erase");
		entries.erase(key);
	}

	//TODO: These should be overloads, to provide a consistent interface.

	//!Creates an integer entry
	void		insert_int(const Tkey& key, const std::string& name, int min, int max, int val) {

		is_possible_key<Tkey>();
		check_unique_key(key);
		entries.insert(
			std::pair<Tkey, uptr_base>(
				key,
				uptr_base(new entry_int(name, min, max, val))
			)
		);
	}

	//!Creates a bool entry.
	void		insert_bool(const Tkey& key, const std::string& name, bool val) {

		is_possible_key<Tkey>();
		check_unique_key(key);
		entries.insert(
			std::pair<Tkey, uptr_base>(
				key,
				uptr_base(new entry_bool(name, val))
			)
		);
	}

	//!Creates a string entry.
	void		insert_string(const Tkey& key, const std::string& name, const std::string& val) {

		is_possible_key<Tkey>();
		check_unique_key(key);
		entries.insert(
			std::pair<Tkey, uptr_base>(
				key,
				uptr_base(new entry_string(name, val))
			)
		);
	}

	//!Creates a void entry.
	void		insert_void(const Tkey& key, const std::string& name) {

		is_possible_key<Tkey>();
		check_unique_key(key);
		entries.insert(
			std::pair<Tkey, uptr_base>(
				key,
				uptr_base(new entry_void(name))
			)
		);
	}

	//Inserts a choice option.

	//!Must be called specifying its Tvalue (menu.template insert_choice<std::string>(key, "value");
	template<typename Tvalue>
	void		insert_choice(const Tkey& _key, const std::string& _name) {

		is_possible_key<Tkey>();
		is_possible_value<Tvalue>();

		entries.insert(
			std::pair<Tkey, uptr_base>(
				_key,
				uptr_base(new entry_choice<Tvalue>(_name))
			)
		);
	}

	//!Inserts a selection with key_sel, value and name for the option identified by "key".
	template<typename Tvalue>
	void		insert_choice(
		const Tkey& _key,
		const std::string& _translation,
		const Tvalue _value
	) {

	std::cout<<"insert choice with key: "<<_key<<" translation: "<<_translation<<" value: "<<_value<<std::endl;

		is_possible_key<Tkey>();
		is_possible_value<Tvalue>();

		check_entry(_key, "key does not exist for insert choice: "+_translation);
		auto& o=entries.at(_key);
		check_type(o->get_type(), types::tchoice, _key, "entry is not of choice type");

		static_cast<entry_choice<Tvalue> *>(o.get())->insert(_value, _translation);
	}

	//!Erases a value (key_sel) from a choice option (key).
	/*
	TODO: Nope, erase by value.
	template<typename Tvalue>
	void		erase_choice(const Tkey& key, const Tkey& key_sel) {

		is_possible_key<Tkey>();
		is_possible_value<Tvalue>();
		check_entry(key, "key does not exist for erase choice");
		auto& o=entries.at(key);
		check_type(o->get_type(), types::tchoice, key, "option is not choice");

		static_cast<entry_choice<Tvalue> *>(o.get())->erase(key_sel);
	}
	*/

	//!Gets the number of choices for a templated option.
	template<typename Tvalue>
	size_t		size_choice(const Tkey& _key) const {

		is_possible_key<Tkey>();
		is_possible_value<Tvalue>();
		check_entry(_key, "key does not exist for size choice");
		auto& o=entries.at(_key);
		check_type(o->get_type(), types::tchoice, _key, "entry is not of choice type");

		return static_cast<entry_choice<Tvalue> *>(o.get())->size();
	}

	//!Browses the option key in the direction dir.
	void		browse(const Tkey& _key, browse_dir _dir) {

		is_possible_key<Tkey>();
		check_entry(_key, "key does not exist for browse");
		entries.at(_key)->browse(_dir);
	}

/*
	template<typename tvalue>
	value	get(const TKey& key) const {

		is_possible_key<Tkey>();
		check_entry(_key, "key does not exist for get");
		auto& o=entries.at(_key);

		switch(o->get_type()) {
			case types::tbool:
				return static_cast<entry_bool *>(o.get())->value;
			break;
			case types::tint:
				return static_cast<entry_int *>(o.get())->value;
			break;
			case types::tstring:
				return static_cast<entry_string *>(o.get())->value;
			break;
			case types::tchoice:
				static_cast<entry_choice<tvalue>*>(o.get())->get_value();
			break;
			case types::tvoid:
				throw options_menu_exception("void choices are not returnable");
			break;
		}
	}
*/
/*

	//!Specifically returns integer values from an integer option.
	//TODO: These should be only get.
	int		get_int(const Tkey& key) const {

		is_possible_key<Tkey>();
		check_entry(key, "key does not exist for get_int");
		const auto& o=entries.at(key);
		check_type(o->get_type(), types::tint, key, "option is not int");
		return static_cast<entry_int *>(o.get())->get_value();
	}

	//!Specifically returns bool values from an bool option.
	bool	get_bool(const Tkey& key) const {

		is_possible_key<Tkey>();
		check_entry(key, "key does not exist for get_bool");
		const auto& o=entries.at(key);
		check_type(o->get_type(), types::tbool, key, "option is not bool");
		return static_cast<entry_bool *>(o.get())->get_value();
	}

	//!Specifically returns string values from a string option.
	std::string	get_string(const Tkey& key) const {

		is_possible_key<Tkey>();
		check_entry(key, "key does not exist for get_string");
		const auto& o=entries.at(key);
		check_type(o->get_type(), types::tstring, key, "option is not string");
		return static_cast<entry_string *>(o.get())->get_value();
	}

	//TODO: There's no "get choice" template.

	//!Returns the string value representation for the current selection in the option.
	std::string	get_str_value(const Tkey& key) const {

		is_possible_key<Tkey>();
		check_entry(key, "key does not exist for get_str_value");
		const auto& o=entries.at(key);
		return o->get_str_value();
	}
*/
	//!Set the value for any key.
	template<typename tvalue>
	void	set(const Tkey& _key, tvalue _value) {

		is_possible_key<Tkey>();
		is_possible_value<tvalue>();

		check_entry(_key, "key does not exist for set");
		auto& o=entries.at(_key);

		auto fail=[_key](const std::string& _msg) {
			std::stringstream ss;
			ss<<_msg<<" for key '"<<_key<<"'";
			throw options_menu_exception(ss.str());
		};

		//Values are assigned through private overloads, to avoid the compiler
		//complaining of invalid assignments (this is compile time, tvalue might
		//be a string and we might be trying to assign to an integer).
		switch(o->get_type()) {
			case types::tstring:
				if(!std::is_same<tvalue, std::string>::value
					&& !std::is_same<tvalue, const char *>::value
				) {
					fail("not a string type entry");
				}

				return assign(_key, _value);

			case types::tbool:
				if(!std::is_same<tvalue, bool>::value) {
					fail("not a boolean type entry");
				}

				return assign(_key, _value);

			case types::tint:
				if(!std::is_same<tvalue, int>::value) {
					fail("not an int type entry");
				}

				return assign(_key, _value);
			break;
			case types::tchoice:

				return assign(_key, _value, o->get_value_type());

			case types::tvoid:
				fail("void choice cannot be assigned");
		}
	}



	//!Returns the option name for the given key.
	std::string	get_name(const Tkey& key) const {

		is_possible_key<Tkey>();
		check_entry(key, "key does not exist for get_name");
		return entries.at(key)->name;
	}

	//!Returns the quantity of entries in a menu.
	size_t		size() const {

		return entries.size();
	}

	//!Gets a vector with the different keys used in entries.

	//!Keys for choices are not returned. This is mostly used to draw the
	//!menu: all keys are returned and then the options can be queried.
	std::vector<Tkey>		get_keys() const {

		std::vector<Tkey> res; //Cannot directly return "keys", as these contain the choices too.
		for(const auto& o : entries) {
			res.push_back(o.first);
		}
		return res;
	}

	private:

	mutable std::vector<Tkey>				keys; //!< Stores all different Tkey keys for duplicate checking.
	std::map<Tkey, std::unique_ptr<base_entry>>		entries;
};

//!Mounts a previously created menu using a JSON value with rapidjson..

/**The first parameter is a rapidJson value, which is supposed to be of array
type. The second is the menu and the third is an empty map that will be returned
filled with an integer for each key. This integer is meant to represent the
translation string used in, for example, the localization class.

The translator parameter is only useful when external localization tools
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

template<typename tkey>
void options_menu_from_json(
	const rapidjson::Value& _root,
	options_menu<tkey>& _target_menu
) {
	const char 	* k_key="key",
				* k_type="type",
				* k_properties="properties",
				* k_min="min",
				* k_max="max",
				* k_value_type="value_type",
				* k_values="values",
				* k_value="value";

	if(!_root.IsArray()) {
		throw std::runtime_error("options_menu_from_json: root node must be an array");
	}

	for(const auto& entry : _root.GetArray()) {

		if(!entry.IsObject()) {
			throw std::runtime_error("options_menu_from_json: entry must be an object");
		}

		if(!entry.HasMember(k_key)) {
			throw std::runtime_error("options_menu_from_json: entry node must have a key value");
		}

		if(!json_is<tkey>(entry[k_key])) {
			throw std::runtime_error("options_menu_from_json: the key value must be an of the template type");
		}

		if(!entry.HasMember(k_type)) {
			throw std::runtime_error("options_menu_from_json: entry node must have a type value");
		}

		if(!entry[k_type].IsString()) {
			throw std::runtime_error("options_menu_from_json: the type value must be an string");
		}

		const std::string entry_type=entry[k_type].GetString();
		const tkey entry_key=json_get<tkey>(entry[k_key]);

		if(entry_type=="choice") {

			if(!entry.HasMember(k_properties)) {
				throw std::runtime_error("options_menu_from_json: choice entry must have a properties node");
			}

			const auto& properties=entry[k_properties];
			if(!properties.IsObject()) {
				throw std::runtime_error("options_menu_from_json: the properties node must be an object");
			}

			if(!properties.HasMember(k_value_type)) {
				throw std::runtime_error("options_menu_from_json: the properties node must have a value_type property");
			}

			if(!properties[k_value_type].IsString()) {
				throw std::runtime_error("options_menu_from_json: value_type must be a string");
			}

			if(!properties.HasMember(k_values)) {
				throw std::runtime_error("options_menu_from_json: the properties node must have a values property");
			}

			if(!properties[k_values].IsArray()) {
				throw std::runtime_error("the values property must be an array");
			}

			//Bufff... Ese "template" está para que el compilador no se grille: es el "template disambiguator",
			//que ayuda a saber que es un método templatizado y no una propiedad seguida de "menor que".
			const std::string value_type=properties[k_value_type].GetString();

			if(value_type=="string") {
				_target_menu.template insert_choice<std::string>(entry_key, "#string_choice_placeholder#");
			}
			else if(value_type=="int") {
				_target_menu.template insert_choice<int>(entry_key, "#int_choice_placeholder#");
			}
			else if(value_type=="bool") {
				_target_menu.template insert_choice<bool>(entry_key, "#bool_choice_placeholder#");
			}
			else {
				throw std::runtime_error("options_menu_from_json: unknown choice type "+value_type);
			}

			for(const auto& choice : properties[k_values].GetArray()) {

				if(!choice.IsObject()) {
					throw std::runtime_error("options_menu_from_json: each choice in 'values' must be an object");
				}

				if(!choice.HasMember(k_value)) {
					throw std::runtime_error("options_menu_from_json: choice in 'values' must have a value property");
				}

				if(value_type=="string") {
					_target_menu.template insert_choice<std::string>(
						entry_key,
						"#string_value_placeholder#",
						choice["value"].GetString()
					);
				}
				else if(value_type=="int") {
					_target_menu.template insert_choice<int>(
						entry_key,
						"#int_value_placeholder",
						choice["value"].GetInt()
					);
				}
				else if(value_type=="bool") {

					bool value=choice["value"].GetBool();

					_target_menu.template insert_choice<bool>(
						entry_key,
						"#bool_value_placeholder",
						value
					);
				}
			}
		}
		else if(entry_type=="int") {

			if(!entry.HasMember(k_properties)) {
				throw std::runtime_error("options_menu_from_json: int entry must have a properties node");
			}

			const auto& properties=entry[k_properties];
			if(!properties.IsObject()) {
				throw std::runtime_error("options_menu_from_json: the properties node must be an object");
			}

			if(!properties.HasMember(k_min)) {
				throw std::runtime_error("options_menu_from_json: the properties node must have a min property");
			}

			if(!properties[k_min].IsInt()) {
				throw std::runtime_error("options_menu_from_json: min property must be an integer");
			}

			if(!properties.HasMember(k_max)) {
				throw std::runtime_error("options_menu_from_json: the properties node must have a min property");
			}

			if(!properties[k_max].IsInt()) {
				throw std::runtime_error("options_menu_from_json: max property must be an integer");
			}

			int minval=properties[k_min].GetInt(),
				maxval=properties[k_max].GetInt();

			_target_menu.insert_int(
				entry_key,
				"#int_placeholder#",
				minval,
				maxval,
				minval
			);
		}
		else if(entry_type=="bool") {

			_target_menu.insert_bool(entry_key, "#bool_placeholder#", true);
		}
		else if(entry_type=="string") {

			_target_menu.insert_string(entry_key, "#string_placeholder#", "#string_placeholder_value#");
		}
		else if(entry_type=="void") {

			_target_menu.insert_void(entry_key, "#void_placeholder#");
		}
		else {

			throw std::runtime_error("options_menu_from_json: unknown entry type "+entry_type);
		}
	}
}

}

