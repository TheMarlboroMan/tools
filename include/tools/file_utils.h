#pragma once

#include <string>
#include <fstream>

#if __has_include(<filesystem>)

	#include <filesystem>
namespace tools { namespace filesystem = std::filesystem;}

#elif __has_include(<experimental/filesystem>)

	#include <experimental/filesystem>

namespace tools {namespace filesystem = std::experimental::filesystem;}
#else

  error "Missing the <filesystem> header."
#endif

namespace tools{

//!Dumps the contents of the file to a string.
std::string	dump_file(const std::string&);

}
