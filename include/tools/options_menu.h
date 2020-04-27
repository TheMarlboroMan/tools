#pragma once

#include "ranged_value.h"
#include "json.h"
#include "algorithm.h"

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
struct options_menu_exception:
	public std::runtime_error {
	//!Class constructor.
	options_menu_exception(const std::string& s)
		:std::runtime_error(s) {
	}
};

//Anonymous namespace, makes this stuff invisible to the outside world.
namespace {

	//!Compile time assertion that a menu will be created with valid key types.
	template<typename t> struct possible_key{static const bool value=false;};
	template<> struct possible_key<int>{static const bool value=true;};
	template<> struct possible_key<std::string>{static const bool value=true;};
	template<typename t> void assert_valid_key() {
		static_assert(possible_key<t>::value, "only integer and string keys are allowed");
	}	

	//!Compile time assertion that a choices will be created with valid value types.
	template<typename t> struct possible_value{static const bool value=false;};
	template<> struct possible_value<bool>{static const bool value=true;};
	template<> struct possible_value<int>{static const bool value=true;};
	template<> struct possible_value<std::string>{static const bool value=true;};
	template<> struct possible_value<const char *>{static const bool value=true;};
	template<typename t> void assert_valid_value() {
		static_assert(possible_value<t>::value, "only integer, string and boolean types are permitted for choices");
	}

	//!Defines the different types of choices.
	enum class types {tchoice, tint, tbool, tstring, tvoid};

	//!Tag dispatching. Returns the type (of type types) that corresponds to a primitive (or string).
	template<typename t> struct type_for{static const types value=types::tvoid;};
	template<> struct type_for<int>{static const types value=types::tint;};
	template<> struct type_for<bool>{static const types value=types::tbool;};
	template<> struct type_for<std::string>{static const types value=types::tstring;};
	template<> struct type_for<const char *>{static const types value=types::tstring;};

	//So close... but does not work with enum types :(.
/*
	struct invalid_value_type{};
	template<typename t> struct value_type_for{static const invalid_value_type value;};
	template<> struct value_type_for<types::tint>{static const int value=0;};
	template<> struct value_type_for<types::tbool>{static const bool value=false;};
	template<> struct value_type_for<types::tstring>{static const std::string value{};};
*/
}

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
completely unique. The type key is the typename of the template. The key
must be printable by streams.

The order of entries and values is determined by the order of insertion.

As stated,there are different types of entries:

	int: used to represent integer values within a range.
	bool: used to represent true or false
	string: used to store strings.
	void: does not store anything, like the access to a submenu.
	choice: used to represent a choice of any given type (string, int, bool...) among many.

Menus can be built by code or from a JSON node using the rapidjson library.

Finally, there is a particular exception "options_menu_exception" thrown for
every error.
*/

template<typename tkey>
class options_menu {

	public:

	enum class browse_dir{next, previous};

	private:

	struct base_entry; //Forward declaration.
	using uptr_base=std::unique_ptr<base_entry>;
	std::vector<uptr_base>	entries;
	
	//!Exception with a key at the end of its message.	
	struct key_exception:
		public options_menu_exception{
		//!Class constructor.
		key_exception(const tkey& _key, const std::string& _msg)
			:options_menu_exception(_msg+" in key '"+key_to_string(_key)+"'") {

		}

		private:

		static std::string key_to_string(const tkey& _k) {
	
			std::stringstream ss;
			ss<<_k;
			return ss.str();
		}
	};

	//!Base class for all entries.
	//TODO: Add some sort of shit in which we can implement callbacks to 
	//foreign entities and dispatch our values.
	struct base_entry {

		tkey					key;

		//!Returns the key.
		const tkey&				get_key() const {return key;}

		//!Returns its type. Internal use.
		virtual types			get_type() const=0;

		//!Returns its value type. Internal use. Its only use is for choice types.
		virtual types			get_value_type() const {return get_type();}

		//!Chooses the next/previous value in the option.
		virtual void			browse(browse_dir)=0;

		//Base constructor.
								base_entry(const tkey& _key)
			:key(_key){
		}
	};

	//!Structure to represent a choice among many.
	template<typename tvalue>
	struct entry_choice
		:public base_entry {

		types				value_type; //The type of the value (tvalue) must be stored in order to be retrieved.
		std::vector<tvalue>	choices; //!< Internal option map.
		std::size_t			current_index=0;	//!< Currently selected key.

		//!Returns the typename value for the current selection (0,1,2 in the example).
		tvalue				get_value() const {

			if(!choices.size()) {
				throw options_menu_exception("choice with no values for get_value");
			}

			return choices.at(current_index);
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
		//TODO: Make the wrapround be optional.
		virtual void 				browse(browse_dir _dir) {

			if(!choices.size()) {
				throw options_menu_exception("choice with no values for browse");
			}

			if(_dir==browse_dir::previous) {

				if(0==current_index) {
					current_index=choices.size()-1;
				}
				else {
					--current_index;
				}
			}
			else {
				++current_index;
				if(current_index == choices.size()) {
					current_index=0;
				}
			}
		}

		bool                    has_value(const tvalue& _value, int& _distance) {
		
			auto it=std::find(
				std::begin(choices),
				std::end(choices),
				_value
			);
			
			_distance=-1;
			if(it==std::end(choices)) {
				return false;
			}

			_distance=std::distance(std::begin(choices), it);
			return true;
		}

		//!Sets the selection by its value.
		void 					set(const tvalue& _value) {

			int index{0};
			if(!has_value(_value, index)) {
				throw options_menu_exception("value does not exist in choice");
			}
			
			current_index=index;
		}

		//!Returns the number of choices.
		size_t				size() const {

			return choices.size();
		}

		//!Inserts a new selection into the option.
		void				insert(const tvalue& _value) {

			int dummy{0};
			if(has_value(_value, dummy)) {
				throw options_menu_exception("value already exists in choice!");
			}

			choices.push_back(_value);
		}

		//!Removes the selection identified by the given value.
		void 				erase(const tvalue& _value) {

			int index{0};
			if(!has_value(_value, index)) {
				throw options_menu_exception("value does not exist for erase");
			}
			
			choices.erase(std::begin(choices)+index);
		}

		//!Constructor.
		entry_choice(const tkey& _key, types _type)
			:base_entry(_key), value_type{_type} {

		}
	};

	//!Represents an integer value within a given range.
	struct entry_int
		:public base_entry {

		ranged_value<int>		value; //!< Ranged value for the option.

		//!Returns the current value as an integer.
		int						get_value() const {return value.get();}
		//!Returns its type. Internal use.
		virtual types			get_type() const {return types::tint;}
		//!Adds or substracts from the value.

		//TODO: Allow for optional wrap around.
		virtual void			browse(browse_dir _dir){
			value+=_dir==browse_dir::next ? 1 : -1;
		}

		//!Constructor of the option.
		entry_int(const tkey& _key, int _value, int _min, int _max)
			:base_entry(_key), value(_min, _max, _value) {
		}
	};

	//Represents a boolean value.
	struct entry_bool
		:public base_entry {
		bool				value; //!< Option value.

		//!Returns the boolean value.
		bool				get_value() const {return value;}

		//!Returns its type. Internal use.
		virtual types			get_type() const {return types::tbool;}

		//!Ignores the parameter and just flips the value.
		virtual void			browse(browse_dir){value=!value;}

		entry_bool(const tkey& _key, bool _value)
			:base_entry(_key), value(_value) {
		}
	};

	//Structure to represent a string.
	struct entry_string
		:public base_entry {
		std::string			value; //!< String value.

		//!Returns the string value.
		std::string			get_value() const {return value;}

		//!Returns its type. Internal use.
		virtual types			get_type() const {return types::tstring;}

		//!Cannot be done.
		virtual void			browse(browse_dir){
			throw key_exception(this->key, "is not browsable");
		}

		//!Constructor.
		entry_string(const tkey& _key, const std::string& _value)
			:base_entry(_key), value(_value) {
		}
	};

	//Structure with no associated value.
	struct entry_void
		:public base_entry {
		//!Does nothing.
		void				get_value() const {}

		//!Returns its type. Internal use only.
		virtual types			get_type() const {return types::tvoid;}


		//!Cannot be done.
		virtual void			browse(browse_dir ){

			throw key_exception(this->key, "is not browsable");
		}

		//!Constructor.
		entry_void(const tkey& _key)
			:base_entry(_key) {
		}
	};

	//!Assignment overloads for the "set" function...
	void	assign(const tkey& _key, int _value) {
		
		static_cast<entry_int *>(get_entry(_key).get())->value=_value;
	}

	void	assign(const tkey& _key, bool _value) {

		static_cast<entry_bool *>(get_entry(_key).get())->value=_value;
	}

	void	assign(const tkey& _key, const std::string& _value) {

		static_cast<entry_string *>(get_entry(_key).get())->value=_value;
	}

	void	assign(const tkey& _key, int _value, types _value_type) {

		if(types::tint != _value_type) {
			throw key_exception(_key, "type must be an integer for choice");
		}

		static_cast<entry_choice<int>*>(get_entry(_key).get())->set(_value);
	}

	void	assign(const tkey& _key, bool _value, types _value_type) {

		if(types::tbool != _value_type) {
			throw key_exception(_key, "type must be boolean for choice");
		}

		static_cast<entry_choice<bool>*>(get_entry(_key).get())->set(_value);
	}

	void	assign(const tkey& _key, const std::string& _value, types _value_type) {

		if(types::tstring != _value_type) {
			throw key_exception(_key, "type must be string for choice");
		}

		static_cast<entry_choice<std::string>*>(get_entry(_key).get())->set(_value);
	}

	//!Overloads for erase_choice.
	void	erase_choice_impl(const tkey& _key, int _value) {

		auto& o=get_entry(_key);
		if(types::tint != o->get_value_type()) {

			throw key_exception(_key, "parameter type does not match for erase_choice_impl");
		}
		static_cast<entry_choice<int> *>(o.get())->erase(_value);
	}

	void	erase_choice_impl(const tkey& _key, bool _value) {

		auto& o=get_entry(_key);
		if(types::tbool != o->get_value_type()) {

			throw key_exception(_key, "parameter type does not match for erase_choice_impl");
		}
		static_cast<entry_choice<bool> *>(o.get())->erase(_value);
	}

	void	erase_choice_impl(const tkey& _key, const std::string& _value) {

		auto& o=get_entry(_key);
		if(types::tstring != o->get_value_type()) {

			throw key_exception(_key, "parameter type does not match for erase_choice_impl");

		}
		static_cast<entry_choice<std::string> *>(o.get())->erase(_value);
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

	//!Returns the entry by the given key. Throws if the key cannot be found.
	const uptr_base&		get_entry(const tkey& _key) const {

		auto it=std::find_if(
			std::begin(entries),
			std::end(entries),
			[_key](const uptr_base& _entry) {
				return _entry->key==_key;
			}
		);

		if(it==std::end(entries)) {
			throw key_exception(_key, "not found");
		}

		return *it;
	}

	//!Returns the entry by the given key. Throws if the key cannot be found.
	uptr_base&		get_entry(const tkey& _key) {

		//That's a mouthful.
		return const_cast<uptr_base&>(const_cast<const options_menu<tkey>*>(this)->get_entry(_key));
	}

	public:

	//!Checks that the option with the key does not exist.
	bool	key_exists(const tkey& _key) const {

		return std::end(entries)!=std::find_if(
			std::begin(entries), 
			std::end(entries), 
			[_key](const uptr_base& _entry) {
				return _entry->key == _key;
			}
		);
	}

	//!Creates an integer entry
	void		insert(const tkey& _key, int _value, int _min, int _max) {

		assert_valid_key<tkey>();
		if(key_exists(_key)) {
			throw key_exception(_key, "key already exists for insert");
		}

		entries.push_back(
			uptr_base(new entry_int(_key, _value, _min, _max))
		);
	}

	//!Creates a bool entry.
	void		insert(const tkey& _key, bool _val) {

		assert_valid_key<tkey>();
		if(key_exists(_key)) {
			throw key_exception(_key, "key already exists for insert");
		}
		entries.push_back(
			uptr_base(new entry_bool(_key, _val))
		);
	}

	//!Creates a string entry. Disambiguation.
	void		insert(const tkey& _key, const char * _val) {
	
		insert(_key, std::string{_val});
	}
	
	//!Creates a string entry.
	void		insert(const tkey& _key, const std::string& _val) {

		assert_valid_key<tkey>();
		if(key_exists(_key)) {
			throw key_exception(_key, "key already exists for insert");
		}

		entries.push_back(
			uptr_base(new entry_string(_key, _val))
		);
	}

	//!Creates a void entry.
	void		insert(const tkey& _key) {

		assert_valid_key<tkey>();
		if(key_exists(_key)) {
			throw key_exception(_key, "key already exists for insert");
		}
		entries.push_back(
			uptr_base(new entry_void(_key))
		);
	}

	//Inserts a choice option.
	template<typename tvalue>
	void		insert(const tkey& _key, const std::vector<tvalue>& _values) {

		assert_valid_key<tkey>();
		assert_valid_value<tvalue>();
		if(key_exists(_key)) {
			throw key_exception(_key, "key already exists for insert_choice");
		}

		entry_choice<tvalue> * new_choice=new entry_choice<tvalue>(_key, type_for<tvalue>::value);
		for(const auto& value : _values) {
			new_choice->insert(value);
		}

		entries.push_back(
			uptr_base(new_choice)
		);
	}

	//!Erases the entry with the given key.
	void		erase(const tkey& _key) {

		assert_valid_key<tkey>();
		if(!key_exists(_key)) {
			throw key_exception(_key, "key does not exist for erase");
		}
		
		entries.erase(
			std::remove_if(
				std::begin(entries),
				std::end(entries),
				[_key](const uptr_base& _entry) {
					return _entry->key==_key;
				}
			),
			std::end(entries)
		);
	}

	//!Inserts a value for the choice option identified by "key".
	template<typename tvalue>
	void		add(const tkey& _key, tvalue _value) {

		assert_valid_key<tkey>();
		auto& o=get_entry(_key);
		if(types::tchoice!=o->get_type()) {

			throw key_exception(_key, "entry is not of choice type");
		}

		if(o->get_value_type()!=type_for<tvalue>::value) {
			throw key_exception(_key, "value type does not match");
		}

		static_cast<entry_choice<tvalue> *>(o.get())->insert(_value);
	}

	//!Disambiguation.
	void		add(const tkey& _key, const char * _value) {
		add<std::string>(_key, std::string{_value});
	}

	//!Erases a value from a choice entry.
	template<typename tvalue>
	void		erase_choice(const tkey& _key, const tvalue& _value) {

		assert_valid_key<tkey>();
		assert_valid_value<tvalue>();
		auto& o=get_entry(_key);
		if(types::tchoice!=o->get_type()) {

			throw key_exception(_key, "option is not a choice for erase_choice");
		}

		auto value_type=o->get_value_type();
		if(value_type!=type_for<tvalue>::value) {

			throw key_exception(_key, "value type does not match for erase_choice");
		}

		erase_choice_impl(_key, _value);
	}

	//!Disambiguate...
	void		erase_choice(const tkey& _key, const char * _value) {
		erase_choice<std::string>(_key, std::string{_value});
	}

	//!Gets the number of choices for a templated option.
	size_t		size_choice(const tkey& _key) const {

		assert_valid_key<tkey>();
		auto& o=get_entry(_key);
		if(types::tchoice!=o->get_type()) {

			throw key_exception(_key, "entry is not of choice type for size_choice");
		}

		switch(o->get_value_type()) {
			case types::tint:		return static_cast<entry_choice<int> *>(o.get())->size();
			case types::tstring:	return static_cast<entry_choice<std::string> *>(o.get())->size();
			case types::tbool:		return static_cast<entry_choice<bool> *>(o.get())->size();
			default:
				throw key_exception(_key, "type mismatch for size_choice");
		}
	}

	//!Browses the option key in the direction dir.
	void		browse(const tkey& _key, browse_dir _dir) {

		assert_valid_key<tkey>();
		get_entry(_key)->browse(_dir);
	}

	//!Specifically returns integer values from an integer option.
	int		get_int(const tkey& _key) const {

		assert_valid_key<tkey>();
		const auto& o=get_entry(_key);

		if(o->get_value_type()!=types::tint) {
			throw key_exception(_key, "choice is not int for get_int");
		}

		//A pure int...
		if(o->get_type()==types::tint) {
			return static_cast<entry_int *>(o.get())->get_value();
		}

		//A choice int...
		return static_cast<entry_choice<int> *>(o.get())->get_value();
	}

	//!Specifically returns bool values from an bool option.
	bool	get_bool(const tkey& _key) const {

		assert_valid_key<tkey>();
		const auto& o=get_entry(_key);
		if(types::tbool != o->get_value_type()) {

			throw key_exception(_key, "choice is not bool for get_bool");
		}		

		if(types::tbool==o->get_type()) {
			return static_cast<entry_bool *>(o.get())->get_value();
		}

		return static_cast<entry_choice<bool> *>(o.get())->get_value();
	}

	//!Specifically returns string values from a string option.
	std::string	get_string(const tkey& _key) const {

		assert_valid_key<tkey>();
		const auto& o=get_entry(_key);
		if(types::tstring != o->get_value_type()) {
			
			throw key_exception(_key, "choice is not string for get_string");
		}		

		if(types::tstring==o->get_type()) {
			return static_cast<entry_string *>(o.get())->get_value();
		}

		return static_cast<entry_choice<std::string> *>(o.get())->get_value();
	}

	//!Set the value for any key.
	template<typename tvalue>
	void	set(const tkey& _key, tvalue _value) {

		assert_valid_key<tkey>();
		assert_valid_value<tvalue>();

		auto& o=get_entry(_key);

		//Values are assigned through private overloads, to avoid the compiler
		//complaining of invalid assignments (this is compile time, tvalue might
		//be a string and we might be trying to assign to an integer).
		
		switch(o->get_type()) {
			case types::tstring:
				if(!std::is_same<tvalue, std::string>::value
					&& !std::is_same<tvalue, const char *>::value
				) {
					throw key_exception(_key, "not a string type entry");
				}

				return assign(_key, _value);

			case types::tbool:
				if(!std::is_same<tvalue, bool>::value) {
					throw key_exception(_key, "not a boolean type entry");
				}

				return assign(_key, _value);

			case types::tint:
				if(!std::is_same<tvalue, int>::value) {
					throw key_exception(_key, "not an int type entry");
				}

				return assign(_key, _value);
			break;
			case types::tchoice:

				return assign(_key, _value, o->get_value_type());

			case types::tvoid:
				throw key_exception(_key, "void choice cannot be assigned");
		}
	}

	//Disambiguation.
	void	set(const tkey& _key, const char * _value) {
		set(_key, std::string{_value});
	}

	//!Returns the quantity of entries in a menu.
	size_t		size() const {

		return entries.size();
	}

	//!Gets a vector with the different keys used in entries.
	std::vector<tkey>		get_keys() const {

		std::vector<tkey> res; //Cannot directly return "keys", as these contain the choices too.
		for(const auto& o : entries) {
			res.push_back(o->get_key());
		}
		return res;
	}
};

//!Mounts a previously created menu using a JSON value with rapidjson..

/**The first parameter is a rapidJson value, which is supposed to be of array
type. The second is the menu and the third is an empty map that will be returned
filled with an integer for each key. This integer is meant to represent the
translation string used in, for example, the localization class.

The examples/menu code includes an example of the menu loaded and working
in text mode.

The structure of the json must be rigid, lest this function will throw.

[			//This is the root key, with an array value.
	{		//An object.
		"type":"choice",	//A templated option
		"key":"10_WINDOW",	//its key is "10_WINDOW!
		"properties" : {
			"value_type":"string",	//with the internal type string
			"values":[		//The choices, their keys and values follow.
				{"value": "800x500"},
				{"value": "1200x750"},
				{"value": "1600x1000"}
			]
		}
	},
	{
		"type":"choice",  //A templated option
		"key":"20_HELP",	//with this key
		"properties":{
			"value_type":"bool",	//internally represented by a boolean
			"values":[		//These are the mapped choices.
				{"value": true},
				{"value": false}
			]
		}
	},
	//Choice options can also represent int values, not seen in this example.

	{
		"type":"string",	//A string option.
		"key":"33_NAME"	//With a key
	},
	{
		"type":"int",		//An integer option
		"key":"25_FILESIZE",	//With a key
		"properties": {
			"min": 1,		//Min value (set by default)
			"max": 9999		//And max value.
		}
	},
	{
		"type":"bool",		//An bool option. By default its value is "true".
		"key":"27_BACKUP"		//With a key
	},
	{
		"type":"void",		//A void option
		"key":"30_EXIT"
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

			//TODO: It would be nice to just add all values up and create
			//everything in one go.

			const std::string value_type=properties[k_value_type].GetString();

			if(value_type=="string") {
				_target_menu.insert(entry_key, std::vector<std::string>{});
			}
			else if(value_type=="int") {
				_target_menu.insert(entry_key, std::vector<int>{});
			}
			else if(value_type=="bool") {
				_target_menu.insert(entry_key, std::vector<bool>{});
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
					_target_menu.add(
						entry_key,
						choice["value"].GetString()
					);
				}
				else if(value_type=="int") {
					_target_menu.add(
						entry_key,
						choice["value"].GetInt()
					);
				}
				else if(value_type=="bool") {

					bool value=choice["value"].GetBool();

					_target_menu.add(
						entry_key,
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

			_target_menu.insert(
				entry_key,
				minval,
				minval,
				maxval
			);
		}
		else if(entry_type=="bool") {

			_target_menu.insert(entry_key, true);
		}
		else if(entry_type=="string") {

			_target_menu.insert(entry_key, "#placeholder_value#");
		}
		else if(entry_type=="void") {

			_target_menu.insert(entry_key);
		}
		else {

			throw std::runtime_error("options_menu_from_json: unknown entry type "+entry_type);
		}
	}
}

}

