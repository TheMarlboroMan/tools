#pragma once

#include <string>
#include <stdexcept>

#include <rapidjson/document.h>
#include <rapidjson/allocators.h>

namespace tools {

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