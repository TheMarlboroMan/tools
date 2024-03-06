#pragma once

#include <rapidjson/document.h>
#include <tools/string_utils.h>
#include <tools/json.h>

#include <iostream>
#include <sstream>
#include <vector>
#include <string>

namespace tools{

//!Interface for configuration (or other kind of files) saved in JSON format.

//!Provides easy access to get different data types from path strings expressed
//!as consecutive key names separated by colons, such as config:video:size.

class json_config_file {

	public:

	//!Returns true if the given path exists.
	bool                has_path(const std::string& ppath) const;

	//!Sets the current filename path.
	void                set_filepath(const std::string& _path) {path=_path;}

	//!Returns an integer from the given path. Will throw if the path does not exist or the value is not of the asked type.
	int 				int_from_path(const std::string& ppath) const {return token_from_path(ppath).GetInt();}

	//!Returns a boolean from the given path. Will throw if the path does not exist or the value is not of the asked type.
	bool 				bool_from_path(const std::string& ppath) const {return token_from_path(ppath).GetBool();}

	//!Returns an string from the given path. Will throw if the path does not exist or the value is not of the asked type.
	std::string 		string_from_path(const std::string& ppath) const {return token_from_path(ppath).GetString();}

	//!Returns a double from the given path. Will throw if the path does not exist or the value is not of the asked type.
	double				double_from_path(const std::string& ppath) const {return token_from_path(ppath).GetDouble();}

	//!Returns a float from the given path. Will throw if the path does not exist or the value is not of the asked type.
	float				float_from_path(const std::string& ppath) const {return token_from_path(ppath).GetFloat();}

	//!Returns full json token (as a vector, or another map) from the given path. Will throw if the path does not exist or the value is not of the asked type.
	const rapidjson::Value&	token_from_path(const std::string& c) const;

	//!Returns full json token (as a vector, or another map) from the given path. Will throw if the path does not exist or the value is not of the asked type.
	rapidjson::Value&		token_from_path(const std::string& c);

	//!Generic function to set the value for a given path. Will throw if there is no token in the path or the value is not assignable.
	template <typename T>
	void 	set(const std::string& k, const T& v) {

		token_from_path(k)=v;
	}

/**
 * sets a vector of trivially copied values.
 */
	template <typename T>
	void 	set_vector(
		const std::string& k, 
		const std::vector<T>& _v
	) {

		rapidjson::Value arr{rapidjson::kArrayType};
		for(const auto& t : _v) {

			arr.PushBack(t, document.GetAllocator());
		}

		token_from_path(k)=arr;
	}

/**
 * sets a vector of json values.
 */
	void 	set_vector(
		const std::string& k, 
		const rapidjson::Value& _vector
	) {

		rapidjson::Value v(_vector, document.GetAllocator());
		token_from_path(k)=v;
	}

	template <typename T>
	void add_to_vector(
		rapidjson::Value& _vector, 
		const T& _value
	) {

		tools::add_to_vector(_vector, _value, document);
	}

	template <typename T>
	void add_to_object(rapidjson::Value& _object, const std::string& _key, const T& _value) {

		tools::json_add_to_object(_object, _key, _value, document);
	}

	void 	set_object(
		const std::string& _k,
		rapidjson::Value& _v
	) {

		if(!_v.IsObject()) {

			std::stringstream ss;
			ss<<"cannot set_object from non-object argument in json_config_file, key was "<<_k;
			throw std::runtime_error(ss.str());
		}

		token_from_path(_k)=_v;
	}


	//add a new path with the given value. Will if the path already exists...
	template <typename T>
	void 	add(
		const std::string& _k,
		const T& _v
	) {

		//decompose the path: first we need to reach for it and remove the last
		//part, which is the keyname...
		auto v=explode(_k, ':');
		const auto keyname=v.back();
		v.pop_back();
		rapidjson::Value * p=&document;
		for(const auto& key : v) {

			if(!p->IsObject()) {

				throw std::runtime_error("unable to locate key "+key+" in path "+_k+": "+key+" is not an object");
			}

			if(!p->HasMember(key.c_str())) {

				rapidjson::Value inner_key(rapidjson::kStringType);
				inner_key.SetString(key.c_str(), key.size(), document.GetAllocator());

				rapidjson::Value inner_value(rapidjson::kObjectType);
				p->AddMember(inner_key, inner_value, document.GetAllocator());
			}

			p=&(p->GetObject()[key.c_str()]);
		}

		if(has_path(_k)) {

			throw std::runtime_error("path already exists!");
		}

		rapidjson::Value key(rapidjson::kStringType);
		key.SetString(keyname.c_str(), keyname.size(), document.GetAllocator());

		rapidjson::Value new_entry{_v};
		p->AddMember(key, new_entry, document.GetAllocator());
	}

	//!Reopens the configuration file and assigns the internal token map.
	//!Same effect as calling "load" with the same filename as a parameter.
	void 			reload();

	//!Loads a configuration file from a string. Will throw if the file cannot
	//!be parsed as json (which actually includes the file not existing).
	void 			load(const std::string&);

	//!Saves the tokens to the file pointed at by the string given in "load".
	//TODO: HOW DOES THIS BEHAVE IF LOAD HAS NOT BEEN CALLED????
	void 			save();

	//!Constructs the parser with the given configuration file. The filename
	//!will be used for subsequent save and load operations. The data of
	//!the file will be readily available when the object is fully constructed.
					json_config_file(const std::string&);

	private:

	std::string			throw_on_non_existing_file(const std::string&);

	rapidjson::Document	document;	//!< Internal data storage.
	std::string			path;	//!< Full path and filename of the current config file.
};

}
