#include <tools/system.h>

#include <cstdio>
#include <iostream>
#include <memory>
#include <cstring>
#include <sys/wait.h>

using namespace tools;

exec_exception::exec_exception(const std::string& _msg)
	:std::runtime_error(_msg) {

}

exec_result tools::exec(const char * _command, size_t _bufsize) {
#ifdef WINCOMPIL

	return {0, "this does not work under windows"};
#else
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

	int status=pclose(pipe);
	if(WIFEXITED(status)) {
		status=WEXITSTATUS(status);
	}
	else if(WIFSIGNALED(status)) {
		status=WTERMSIG(status);
	}
	else if(WIFSTOPPED(status)) {
		status=WSTOPSIG(status);
	}

	return {status, result};
#endif
}
