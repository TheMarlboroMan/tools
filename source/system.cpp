#include <cstdio>
#include <iostream>
#include <memory>
#include <cstring>

#include "system.h"

using namespace tools;

exec_exception::exec_exception(const std::string& _msg)
	:std::runtime_error(_msg) {

}

exec_result tools::exec(const char * _command, size_t _bufsize) {

	auto pipe=popen(_command, "r");
	if(!pipe) {
		throw exec_exception(
			std::string("pipe could not be opened for command ")+_command
		);
	}
	
	std::unique_ptr<char[]> buffer(new char[_bufsize]);

	memset(buffer.get(), 0, _bufsize);
	std::string result;
	while(fgets(buffer.get(), _bufsize, pipe) != nullptr) {
		result+=buffer.get();
	}

	return {pclose(pipe), result};
}