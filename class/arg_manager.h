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

	int 			find_index(const char * p_char) const;
	int 			find_index(t_arg const& p_find_index) const;
	void 			init(int argc, char ** argv);
	size_t 			size() const {return data.size();}
	const 			t_arg get_argument(unsigned int p_arg) const;
	const 			t_arg_list& get_data() const {return data;}
	bool			exists(const char * p_char) const {return find_index(p_char)!=-1;}

	//TODO: Mejor, un método al que pasemos un callable.. Y que itere.
	std::string 		get_value(const char *c, const char delimiter='=') const;
	std::string 		get_value(const std::string& c, const char delimiter='=') const;

	private:

	t_arg_list 		data;
};

}

#endif
