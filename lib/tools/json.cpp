#include "../../include/tools/json.h"

#include <rapidjson/writer.h>
#include <rapidjson/error/en.h>
#include <rapidjson/stringbuffer.h>

using namespace tools;

rapidjson::Document tools::parse_json_string(const std::string& _json_str) {

	using namespace rapidjson;
	Document json;
	json.Parse<kParseNoFlags>(_json_str.c_str());
	if(json.HasParseError()) {

		throw parse_json_string_exception(
			std::string("json parser error : ")
			+GetParseError_En(json.GetParseError())
			+" in offset : "
			+std::to_string(json.GetErrorOffset())
			+" for document '"
			+_json_str
			+"'"
		);
	}
	return json;
}

std::string	tools::json_str(const rapidjson::Value& _doc, const std::string& _k) {

	if(!_doc.HasMember(_k.c_str())) {
		throw std::runtime_error(
			std::string("Invalid json key ")
			+_k
		);
	}

	return _doc[_k.c_str()].GetString();
}

int	tools::json_int(const rapidjson::Value& _doc, const std::string& _k) {

	if(!_doc.HasMember(_k.c_str())) {
		throw std::runtime_error(
			std::string("Invalid json key ")
			+_k
		);
	}

	return _doc[_k.c_str()].GetInt();
}

namespace tools {

template<> 
bool json_is<std::string>(const rapidjson::Value& _val) {return _val.IsString();}

template<> 
bool json_is<int>(const rapidjson::Value& _val) {return _val.IsInt();}

template<> 
bool json_is<bool>(const rapidjson::Value& _val) {return _val.IsBool();}

template<> 
bool json_is<double>(const rapidjson::Value& _val) {return _val.IsDouble();}

template<> 
std::string json_get<std::string>(const rapidjson::Value& _val) {return _val.GetString();}

template<> 
int json_get<int>(const rapidjson::Value& _val) {return _val.GetInt();}

template<> 
bool json_get<bool>(const rapidjson::Value& _val) {return _val.GetBool();}

template<> 
double json_get<double>(const rapidjson::Value& _val) {return _val.GetDouble();}
}
