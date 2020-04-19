#pragma once

#include <string>
#include <stdexcept>
#include <iostream>

#include <rapidjson/document.h>
#include <rapidjson/allocators.h>

namespace tools {

//I am not sure if rapidjson already has those. I read something in v1.1 but 
//could not get it to work. Anyway, these are templates that check if a 
//rapidjson value is of the given type, which comes in handy when we are dealing
//with templated types. Please, note that all specializations are in the 
//implementation file!.
template<typename tkey> 
bool json_is(const rapidjson::Value&) {throw std::runtime_error("unsupported type to tools::json_is");}
template<> bool json_is<std::string>(const rapidjson::Value& _val);
template<> bool json_is<int>(const rapidjson::Value& _val);
template<> bool json_is<bool>(const rapidjson::Value& _val);
template<> bool json_is<double>(const rapidjson::Value& _val);

//These are the same as above, but for retrieving values.
template<typename tkey> 
tkey json_get(const rapidjson::Value&) {throw std::runtime_error("unsupported type to tools::json_get");}
template<> std::string json_get<std::string>(const rapidjson::Value& _val);
template<> int json_get<int>(const rapidjson::Value& _val);
template<> bool json_get<bool>(const rapidjson::Value& _val);
template<> double json_get<double>(const rapidjson::Value& _val);


//!Exception to be thrown when "parse_json_string" fails.
class parse_json_string_exception
	:public std::runtime_error {
	public:

	//!Class constructor.
			parse_json_string_exception(const std::string& _m)
				:std::runtime_error(_m) {}
};

//!Parses a string as a JSON document. Throws if cannot parse.
rapidjson::Document 	parse_json_string(const std::string&);

//!Returns the string in the key _k from the json element. Throws if the key does not exist.
std::string				json_str(const rapidjson::Value&, const std::string& _k);

//!Returns the integer in the key _k from the json element. Throws if the key does not exist.
int						json_int(const rapidjson::Value&, const std::string& _k);

//!Creates a json string value.
template<typename A>
rapidjson::Value		json_string(const std::string& _key, A& _allocator) {

	rapidjson::Value key(rapidjson::kStringType);
	key.SetString(_key.c_str(), _key.size(), _allocator);
	return key;
}

}
