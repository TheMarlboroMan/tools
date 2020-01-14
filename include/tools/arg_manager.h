#pragma once

#include "string_utils.h"

#include <vector>
#include <string>
#include <stdexcept>

namespace tools{

//!Exception thrown by the arg_manager.
class arg_manager_exception:
	public std::runtime_error {
	//!Class constructor.
	public:
	arg_manager_exception(const std::string& _m)
		:std::runtime_error(_m) {}
};

//!Argument manager. Facilitates handling of command line arguments.
class arg_manager {

	private:
	typedef std::vector<std::string> t_arg_list;
	typedef std::string t_arg;
	
	public:

	//!Class constructor mimicking the main() entrypoint.
				arg_manager(int argc, char ** argv);

	//!Returns the count of program arguments (program name included).
	size_t 			size() const {return data.size();}

	//!Returns the argument in the given index. Will throw if the index is invalid.
	const t_arg 		get_argument(unsigned int) const;

	//!Returns the argument that follows the one that matches the given 
	//!value (will return "b" if called with "a" in "a.out a b c". 
	//!Will throw if no argument follows or if the original argument
	//!could not be found.
	const t_arg 		get_following(const t_arg&) const;

	//!Returns the full argument list (a vector type).
	const t_arg_list& 	get_data() const {return data;}

	//!Searches for the index of the given argument. Will return -1 if the argument does not exist.
	int 			find_index(const t_arg&) const;
	
	//!Checks if a single exists argument in format ./executable.out arg1 arg2 arg3
	bool			exists(const t_arg& v) const {return find_index(v)!=-1;}

	//!Checks if a single argument in format ./executable.out arg1=val1 arg2=val2 arg3=val3
	bool			value_exists_for(const t_arg& v) const {return find_index_value(v)!=-1;}

	//!Checks if there is an argument following the given one.
	bool			arg_follows(const t_arg& v) const;

	//!Returns the value of the argument if found. Argument is understood as 
	//!argument + delimiter + value, as in val=yes. Will throw if the value
	//!does not exist.
	std::string 		get_value(const t_arg& c, const char delimiter='=') const;

	private:

	//!Returns the index of the argument that matches partially the value (will 
	//!return the index of "test" if called with "te". I am not sure this is of
	//!any use except as a helper to value_exists_for.
	int 			find_index_value(const t_arg&) const;

	//!Fills the internal data structures of the class. 
	void 			init(int argc, char ** argv);

	t_arg_list 		data;
};

}
