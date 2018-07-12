#ifndef TOOLS_DNOT_CONFIG_FILE_H
#define TOOLS_DNOT_CONFIG_FILE_H

#include "dnot_parser.h"
#include <iostream>

namespace tools
{

//!Interface for configuration (or other kind of files) saved in dnot format.

//!Provides easy access to get different data types from path strings expressed
//!as consecutive key names separated by colons, such as config:video:size.

class dnot_config_file {

	public:

	//!Returns an integer from the given path. Will throw if the path does not exist or the value is not of the asked type.
	int 				int_from_path(const std::string& ppath) const {return token_from_path(ppath).get_int();}

	//!Returns a boolean from the given path. Will throw if the path does not exist or the value is not of the asked type.
	bool 				bool_from_path(const std::string& ppath) const {return token_from_path(ppath).get_bool();}

	//!Returns an string from the given path. Will throw if the path does not exist or the value is not of the asked type.
	std::string 			string_from_path(const std::string& ppath) const {return token_from_path(ppath).get_string();}

	//!Returns a double from the given path. Will throw if the path does not exist or the value is not of the asked type.
	double				double_from_path(const std::string& ppath) const {return token_from_path(ppath).get_double();}

	//!Returns a float from the given path. Will throw if the path does not exist or the value is not of the asked type.
	float				float_from_path(const std::string& ppath) const {return token_from_path(ppath).get_float();}

	//!Returns full dnot token (as a vector, or another map) from the given path. Will throw if the path does not exist or the value is not of the asked type.
	const tools::dnot_token&	token_from_path(const std::string& c) const;

	//!Returns full dnot token (as a vector, or another map) from the given path. Will throw if the path does not exist or the value is not of the asked type.
	tools::dnot_token&		token_from_path(const std::string& c);

	//!Generic function to set the value for a given path. Will throw if there is no token in the path or the value is not assignable.
	template <typename T>	void 	set(const std::string& k, const T& v) {

		auto& tok=token_from_path(k);
		tok.set(v);
	}

	//!Reopens the configuration file and assigns the internal token map.
	//!Same effect as calling "load" with the same filename as a parameter.
	void 			reload();

	//!Loads a configuration file from a string. Will throw if the file cannot
	//!be parsed as dnot (which actually includes the file not existing).
	void 			load(const std::string&);

	//!Saves the tokens to the file pointed at by the string given in "load".
	//TODO: HOW DOES THIS BEHAVE IF LOAD HAS NOT BEEN CALLED????
	void 			save();

	//!Constructs the parser with the given configuration file. The filename
	//!will be used for subsequent save and load operations. The data of 
	//!the file will be readily available when the object is fully constructed.
				dnot_config_file(const std::string&);

	private:

	tools::dnot_token	token;	//!< Internal data storage.
	std::string		path;	//!< Full path and filename of the current config file.
};

}
#endif
