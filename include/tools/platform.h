#pragma once

#include <string>

namespace tools{

#ifdef WINBUILD

const std::string			newline="\r\n";

#else

const std::string			newline="\n";

#endif
};
