#ifndef TOOLS_SYSTEM_H
#define TOOLS_SYSTEM_H

#include <string>
#include <stdexcept>

namespace tools {

class exec_exception:	
	public std::runtime_error {
	public:
		exec_exception(const std::string&);
};

struct exec_result {
	int 			code;
	std::string		output;
};

exec_result exec(const char *, size_t=256);

}

#endif