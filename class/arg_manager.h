#ifndef TOOLS_ARG_MANAGER_H
#define TOOLS_ARG_MANAGER_H

#include <vector>
#include <string>
#include <stdexcept>
#include "../source/string_utils.h"

/*
Una clase simple que almacenará los data que se pasen al programa.
*/

namespace tools
{

class arg_manager
{
	private:
	typedef std::vector<std::string> t_arg_list;
	typedef std::string t_arg;
	
	public:

				arg_manager() {}
				arg_manager(int argc, char ** argv);

	void 			init(int argc, char ** argv);
	size_t 			size() const {return data.size();}

	const t_arg 		get_argument(unsigned int) const;
	const t_arg 		get_following(const t_arg&) const;
	const t_arg_list& 	get_data() const {return data;}


	//!Will return -1 if the argument does not exist.
	int 			find_index(const t_arg&) const;
	int 			find_index_value(const t_arg&) const;

	//!Checks if a single argument in format ./executable.out arg1 arg2 arg3
	bool			exists(const t_arg& v) const {return find_index(v)!=-1;}
	//!Checks if a single argument in format ./executable.out arg1=val1 arg2=val2 arg3=val3
	bool			value_exists_for(const t_arg& v) const {return find_index_value(v)!=-1;}
	//!Checks if there is an argument following the given one.
	bool			arg_follows(const t_arg& v) const;

	//TODO: Mejor, un método al que pasemos un callable.. Y que itere.
	std::string 		get_value(const t_arg& c, const char delimiter='=') const;

	private:

	t_arg_list 		data;
};

}

#endif
