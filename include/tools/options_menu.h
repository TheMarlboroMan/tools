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

Each entry has a name. This name is translatable with the "translate" method.

Menus can be built by code or from a JSON node using the rapidjson library.

Finally, there is a particular exception "options_menu_exception" thrown for
every error.
*/

//Forward.
template<typename Tkey> class options_menu;

//TODO: Document.
template<typename tkey, typename ttranslation_id>
class options_menu_translator {

	public:

	void	add_id_to_text(const ttranslation_id& _tid, const std::string& _text) {

		id_to_text[_tid]=_text;
	}

	void	load_translation_ids(const rapidjson::Value& _root) {

		const char 	* k_key="key",
					* k_translation="translation",
					* k_type="type",
					* k_properties="properties",
					* k_values="values";

		if(!_root.IsArray()) {
			throw std::runtime_error("load_translation_ids: root node must be an array ");
		}

		for(const auto& val : _root.GetArray()) {

			if(!val.IsObject()) {
				throw std::runtime_error("load_translation_ids: entry must be an object");
			}

			if(!val.HasMember(k_translation)) {
				throw std::runtime_error("load_translation_ids: entry must have a translation value");
			}

			//Nooope.
			if(!json_is<ttranslation_id>(val[k_translation])) {
				throw std::runtime_error("load_translation_ids: the translation value must be of the templated type");
			}

			if(!val.HasMember(k_type)) {
				throw std::runtime_error("load_translation_ids: entry must have a type value");
			}

			if(!val[k_type].IsString()) {
				throw std::runtime_error("load_translation_ids: the type value must be an string");
			}

			//Add the translation for the entry...
			const tkey entry_key=json_get<tkey>(val[k_key]);
			key_to_id[entry_key]=json_get<ttranslation_id>(val[k_translation]);

			//Only choices have translations...
			const std::string entry_type=val[k_type].GetString();
			if(entry_type=="choice") {

				if(!val.HasMember(k_properties)) {
					throw std::runtime_error("load_translation_ids: entry of type 'choice' must have a properties node");
				}

				const auto& properties=val[k_properties];
				if(!properties.IsObject()) {
					throw std::runtime_error("load_translation_ids: properties node must be an object for entry of type 'choice'");
				}

				if(!properties.HasMember(k_values)) {
					throw std::runtime_error("load_translation_ids: properties must have a values property for entry of type 'choice'");
				}

				if(!properties[k_values].IsArray()) {
					throw std::runtime_error("load_translation_ids: values property must be an array for entry of type 'choice'");
				}

				for(const auto& choice : properties[k_values].GetArray()) {

					if(!choice.IsObject()) {
						throw std::runtime_error("load_translation_ids: each choice in 'values' must be an object");
					}

					if(!choice.HasMember(k_key)) {
						throw std::runtime_error("load_translation_ids: choice in 'values' must have a key property");
					}

					if(!json_is<tkey>(choice[k_key])) {
						throw std::runtime_error("load_translation_ids: choice key must be of the template type");
					}

					if(!choice.HasMember(k_translation)) {
						throw std::runtime_error("load_translation_ids: choice in 'values' must have a translation property");
					}

					if(!json_is<ttranslation_id>(choice[k_translation])) {
						throw std::runtime_error("load_translation_ids: choice translation property must be of the template type");
					}

					const tkey value_key=json_get<tkey>(choice[k_key]);
					key_to_id[value_key]=json_get<ttranslation_id>(val[k_translation]);
				}
			}
		}
	}

	private:

	std::map<tkey, ttranslation_id>		key_to_id;
	std::map<ttranslation_id, std::string> 		id_to_text;

	friend class	options_menu<tkey>;
};

template<typename Tkey>
class options_menu {

	public:

	enum class browse_dir{next, previous};

	private:

	//Assignment overloads for the "set" function. Read "set" to get it.
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

	//!Defines the different types of choices.
	enum class types {tchoice, tint, tbool, tstring, tvoid};

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
		//!Chooses the next/previous value in the option.
		virtual void			browse(browse_dir)=0;
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
			std::string		str_value; 	//!< Display name.
		};

		std::map<Tkey, choice>	choices; //!< Internal option map.
		Tkey				current_key;	//!< Currently selected key.

		//!Returns the typename value for the current selection (0,1,2 in the example).
		Tvalue				get_value() const {
			check_entrys("option with no choices for get_value");
			return choices.at(current_key).value;
		}

		//!Returns the string that represents the current selection ("txt", "html" or "latex" in the example).
		virtual std::string		get_str_value() const {
			check_entrys("option with no choices for get_str_value");
			return choices.at(current_key).str_value;
		}

		//!Returns the type of this class.
		virtual types				get_type() const {
			return types::tchoice;
		}

		//!Changes the current selection.

		//!The choices wrap around.
		virtual void 				browse(browse_dir _dir) {
			auto it=choices.find(current_key);

			if(_dir==browse_dir::previous) {
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

		//!Internal value comparison functions.

		//This is actually fun: if we do "set_by_value" with a const char *
		//the template code will try to call == on it, which will not do what
		//we want. However, we can force type conversion with these helpers.
		bool                    compare_values(bool _a, bool _b) const {return _a==_b;}
		bool                    compare_values(int _a, int _b) const {return _a==_b;}
		bool                    compare_values(const std::string& _a, const std::string _b) const {return _a==_b;}

		//!Sets the selection by its value.
		void 					set_by_value(const Tvalue& _value) {

			for(const auto& s : choices) {

				if(compare_values(s.second.value, _value)) {
					current_key=s.first;
					return;
				}
			}

			std::stringstream ss;
			ss<<"value does not exist for set_by_value '"<<_value<<"'";
			throw options_menu_exception(ss.str());
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
		void				check_entrys(const std::string& msg) const {
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

				t.replace(s.first, s.second.str_value);
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
		virtual std::string		get_str_value() const {return std::to_string(value.get());}
		//!Returns the current value as an integer.
		int						get_value() const {return value.get();}
		//!Returns its type. Internal use.
		virtual types			get_type() const {return types::tint;}
		//!Adds or substracts from the value.
		virtual void			browse(browse_dir _dir){
			value+=_dir==browse_dir::next ? 1 : -1;
		}

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
		virtual std::string		get_str_value() const {return std::to_string(value);}
		//!Returns the boolean value.
		bool				get_value() const {return value ? "true" : "false";}
		//!Returns its type. Internal use.
		virtual types			get_type() const {return types::tbool;}
		//!Ignores the parameter and just flips the value.
		virtual void			browse(browse_dir){value=!value;}
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
		virtual std::string		get_str_value() const {return value;}
		//!Returns its type. Internal use.
		virtual types			get_type() const {return types::tstring;}
		//!Has no effect. Values of this type are manually set.
		virtual void			browse(browse_dir){}
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
		virtual std::string		get_str_value() const {return "";}
		//!Returns its type. Internal use only.
		virtual types			get_type() const {return types::tvoid;}
		//!Does nothing.
		virtual void			browse(browse_dir ){}
		//!Does nothing.
		virtual void			translate(const translation_struct&){}
		//!Constructor.
		entry_void(const std::string& n):base_entry(n) {}
	};

	//!Internal use. Checks that the option with the key does not exist.
	void	check_entry(const Tkey& key, const std::string& msg) const {

		if(!entries.count(key)) throw options_menu_exception(msg);
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

	//!Validates of the types are the same. Throws when they are not. Internal use.
	void		check_type(types ot, types t, const Tkey& _key, const std::string& msg) const {

		if(ot!=t) {
			std::stringstream ss;
			ss<<msg<<" for key '"<<_key<<"'";
			throw options_menu_exception(ss.str());
		}
	}

	public:

	//TODO: These should be overloads, to provide a consistent interface.

	//!Creates an integer option.
	void		insert_int(const Tkey& key, const std::string& name, int min, int max, int val) {

		check_unique_key(key);
		entries.insert(
			std::pair<Tkey, uptr_base>(
				key,
				uptr_base(new entry_int(name, min, max, val))
			)
		);
	}

	//!Creates a bool option.
	void		insert_bool(const Tkey& key, const std::string& name, bool val) {

		check_unique_key(key);
		entries.insert(
			std::pair<Tkey, uptr_base>(
				key,
				uptr_base(new entry_bool(name, val))
			)
		);
	}

	//!Creates a string option.
	void		insert_string(const Tkey& key, const std::string& name, const std::string& val) {

		check_unique_key(key);
		entries.insert(
			std::pair<Tkey, uptr_base>(
				key,
				uptr_base(new entry_string(name, val))
			)
		);
	}

	//!Creates a void option.
	void		insert_void(const Tkey& key, const std::string& name) {

		check_unique_key(key);
		entries.insert(
			std::pair<Tkey, uptr_base>(
				key,
				uptr_base(new entry_void(name))
			)
		);
	}

	//!Erases the option with the given key.
	void		erase(const Tkey& key) {

		check_entry(key, "key does not exist for erase");
		entries.erase(key);
	}

	//Inserts a choice option.

	//!Must be called specifying its Tvalue (menu.template insert_choice<std::string>(key, "value");
	template<typename Tvalue>
	void		insert_choice(const Tkey& key, const std::string& name) {

		check_unique_key(key);
		entries.insert(
			std::pair<Tkey, uptr_base>(
				key,
				uptr_base(new entry_choice<Tvalue>(name))
			)
		);
	}

	//!Inserts a selection with key_sel, value and name for the option identified by "key".
	template<typename Tvalue>
	void		insert_choice(
		const Tkey& key,
		const Tkey& key_sel,
		const std::string& translation,
		const Tvalue value
	) {

		check_unique_key(key_sel);
		check_entry(key, "key does not exist for insert choice: "+translation);
		auto& o=entries.at(key);
		check_type(o->get_type(), types::tchoice, key, "option is not choice");
		static_cast<entry_choice<Tvalue> *>(o.get())->insert(key_sel, value, translation);
	}

	//!Erases a value (key_sel) from a choice option (key).
	template<typename Tvalue>
	void		erase_choice(const Tkey& key, const Tkey& key_sel) {

		check_entry(key, "key does not exist for erase choice");
		auto& o=entries.at(key);
		check_type(o->get_type(), types::tchoice, key, "option is not choice");
		static_cast<entry_choice<Tvalue> *>(o.get())->erase(key_sel);
	}

	//!Sets the value of a choice option (key) by its known key (key_sel).
	template<typename Tvalue>
	void		set_by_key_choice(const Tkey& key, const Tkey& key_sel) {

		check_entry(key, "key does not exist for set_by_key choice");
		auto& o=entries.at(key);
		check_type(o->get_type(), types::tchoice, key, "option is not choice");
		static_cast<entry_choice<Tvalue> *>(o.get())->set_by_key(key_sel);
	}

	//!Gets the number of choices for a templated option.
	template<typename Tvalue>
	size_t		size_choice(const Tkey& key) const {

		check_entry(key, "key does not exist for size choice");
		auto& o=entries.at(key);
		check_type(o->get_type(), types::tchoice, key, "option is not choice");
		return static_cast<entry_choice<Tvalue> *>(o.get())->size();
	}

	//!Browses the option key in the direction dir.
	void		browse(const Tkey& key, browse_dir _dir) {

		check_entry(key, "key does not exist for browse");
		entries.at(key)->browse(_dir);
	}

/*
	template<typename tvalue>
	value	get(const TKey& key) const {

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


	//!Specifically returns integer values from an integer option.
	//TODO: These should be only get.
	int		get_int(const Tkey& key) const {

		check_entry(key, "key does not exist for get_int");
		const auto& o=entries.at(key);
		check_type(o->get_type(), types::tint, key, "option is not int");
		return static_cast<entry_int *>(o.get())->get_value();
	}

	//!Specifically returns bool values from an bool option.
	bool	get_bool(const Tkey& key) const {

		check_entry(key, "key does not exist for get_bool");
		const auto& o=entries.at(key);
		check_type(o->get_type(), types::tbool, key, "option is not bool");
		return static_cast<entry_bool *>(o.get())->get_value();
	}

	//TODO: There's no "get choice"...

	//!Specifically returns string values from a string option.
	std::string	get_string(const Tkey& key) const {

		check_entry(key, "key does not exist for get_string");
		const auto& o=entries.at(key);
		check_type(o->get_type(), types::tstring, key, "option is not string");
		return static_cast<entry_string *>(o.get())->get_value();
	}

	//!Returns the string value representation for the current selection in the option.
	std::string	get_str_value(const Tkey& key) const
	{
		check_entry(key, "key does not exist for get_str_value");
		const auto& o=entries.at(key);
		return o->get_str_value();
	}

	//!Set the value for any key.
	template<typename tvalue>
	void	set(const Tkey& _key, tvalue _value) {

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
			break;

			case types::tbool:
				if(!std::is_same<tvalue, bool>::value) {
					fail("not a boolean type entry");
				}
			break;
			case types::tint:
				if(!std::is_same<tvalue, int>::value) {
					fail("not an int type entry");
				}
			break;
			case types::tchoice:
			{
				auto choice=static_cast<entry_choice<tvalue>*>(o.get());

				if(!choice->size()) {
					fail("no values in the choide");
				}

				if(!std::is_same<decltype(_value), decltype(std::begin(choice->choices)->second.value)>::value) {
					fail("provided value is not of the intended type");
				}

				choice->set_by_value(_value);
				return;
			}
			case types::tvoid:
				fail("void choice cannot be assigned");
		}

		assign(_key, _value);
	}

	//!Returns the option name for the given key.
	std::string	get_name(const Tkey& key) const {

		check_entry(key, "key does not exist for get_name");
		return entries.at(key)->name;
	}

	//!Translates the full menu.

	//!The vector must contain all pairs of Tkey and string values for the
	//!complete menu.
	template<typename ttranslation_id>
	void		translate(const options_menu_translator<Tkey, ttranslation_id>& _translator) {

		std::vector<translation_struct> tsv;
		for(const auto &pair : _translator.key_to_id) {
			tsv.push_back({pair.first, _translator.id_to_text.at(pair.second)});
		}

		for(const auto& ts : tsv) {
			for(auto& o : entries) {
				ts.replace(o.first, o.second->name);
				o.second->translate(ts);
			}
		}
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

				if(!choice.HasMember(k_key)) {
					throw std::runtime_error("options_menu_from_json: choice in 'values' must have a key property");
				}

				if(!json_is<tkey>(choice[k_key])) {
					throw std::runtime_error("options_menu_from_json: choice key must be of the template type");
				}

				if(!choice.HasMember(k_value)) {
					throw std::runtime_error("options_menu_from_json: choice in 'values' must have a value property");
				}

				const tkey value_key=json_get<tkey>(choice[k_key]);

				if(value_type=="string") {
					_target_menu.template insert_choice<std::string>(
						entry_key,
						value_key,
						"#string_value_placeholder#",
						choice["value"].GetString()
					);
				}
				else if(value_type=="int") {
					_target_menu.template insert_choice<int>(
						entry_key,
						value_key,
						"#int_value_placeholder",
						choice["value"].GetInt()
					);
				}
				else if(value_type=="bool") {

					bool value=choice["value"].GetBool();

					_target_menu.template insert_choice<bool>(
						entry_key,
						value_key,
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

