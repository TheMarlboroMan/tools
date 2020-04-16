#include <tools/json_config_file.h>
#include <tools/string_utils.h>
#include <tools/file_utils.h>
#include <tools/json.h>

#include <rapidjson/stringbuffer.h>
#include <rapidjson/writer.h>

#include <map>

using namespace tools;

json_config_file::json_config_file(const std::string& _path)
try
	:document{
		parse_json_string(
			file_exists(_path)
				? dump_file(_path)
				: throw_on_non_existing_file(_path)
		)
	},
	path(_path) {

}
catch(std::runtime_error& e) {
	throw std::runtime_error(std::string("json_config_file: error starting configuration ")+_path+" : "+e.what());
}

void json_config_file::load(const std::string& _path) {

	try {
		document=parse_json_string(dump_file(_path));
		path=_path;
	}
	catch(std::runtime_error& e) {
		throw std::runtime_error(std::string("json_config_file: error loading configuration ")+_path+" : "+e.what());
	} 
}

std::string json_config_file::throw_on_non_existing_file(const std::string& _path) {

	throw std::runtime_error{std::string{"file "}+_path+"does not exist"};
	return path;
}

void json_config_file::reload() {

	try {
		document=parse_json_string(dump_file(path));
	}
	catch(std::runtime_error& e) {
		throw std::runtime_error(std::string("json_config_file: error reloading configuration ")+path+" : "+e.what());
	} 
}

void json_config_file::save() {

	rapidjson::StringBuffer stringbuffer;
	rapidjson::Writer<rapidjson::StringBuffer> writer(stringbuffer);
	document.Accept(writer);
	
	std::ofstream f(path);
	f<<stringbuffer.GetString();
}

const rapidjson::Value& json_config_file::token_from_path(const std::string& _path) const {

	const rapidjson::Value * p=&document;
	auto v=explode(_path, ':');
	for(const auto& key : v) {

		if(!p->HasMember(key.c_str())) {
			throw std::runtime_error("unable to locate key "+key+" in path "+_path);
		}

		if(!p->IsObject()) {
			throw std::runtime_error("unable to locate key "+key+" in path "+_path+": "+key+" is not an object");
		}

		p=&(p->GetObject()[key.c_str()]);
	}

	return *p;
}

rapidjson::Value& json_config_file::token_from_path(const std::string& _path) {

	rapidjson::Value * p=&document;
	auto v=explode(_path, ':');
	for(const auto& key : v) {
		try {
		if(!p->HasMember(key.c_str())) {
				std::runtime_error("unable to locate key "+key+" in path "+_path);
			}

			if(!p->IsObject()) {
				std::runtime_error("unable to locate key "+key+" in path "+_path+": "+key+" is not an object");
			}

			p=&(p->GetObject()[key.c_str()]);
		}
		catch(std::exception& e) {
			throw std::runtime_error("unable to locate key "+key+" in path "+_path);
		}
	}

	return *p;
}
