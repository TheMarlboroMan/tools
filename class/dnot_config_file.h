#ifndef TOOLS_DNOT_CONFIG_FILE_H
#define TOOLS_DNOT_CONFIG_FILE_H

#include "dnot_parser.h"
#include <iostream>

namespace tools
{

class dnot_config_file
{
	////////////////////////////////
	// Public interface...

	public:

	//Multipurpose interface.
	int 				int_from_path(const std::string& ppath) const {return token_from_path(ppath).get_int();}
	bool 				bool_from_path(const std::string& ppath) const {return token_from_path(ppath).get_bool();}
	std::string 			string_from_path(const std::string& ppath) const {return token_from_path(ppath).get_string();}
	const tools::dnot_token&	token_from_path(const std::string& c) const;
	tools::dnot_token&		token_from_path(const std::string& c);
	template <typename T>	void 	set(const std::string& k, const T& v)
	{
		auto& tok=token_from_path(k);
		tok.set(v);
	}

	void reload();
	void load(const std::string&);
	void save();

	dnot_config_file(const std::string&);

	private:

	tools::dnot_token				token;
	std::string					path;
};

}
#endif
