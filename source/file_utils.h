#ifndef TOOLS_FILE_UTILS_H
#define TOOLS_FILE_UTILS_H

#include <string>
#include <fstream>

namespace tools
{

bool		file_exists(const std::string&);
std::string	dump_file(const std::string&);

}

#endif
