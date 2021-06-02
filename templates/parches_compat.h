#ifndef MULTIPLATAFORM_PATCHES_COMPAT_H
#define MULTIPLATAFORM_PATCHES_COMPAT_H

#include <string>
#include <sstream>

namespace compat
{

#ifdef WINCOMPIL
template <typename T> std::string to_string(const T& n)
{
	std::stringstream ss;
	ss<<n;
	return ss.str();
}
#endif
#ifndef WINCOMPIL
template <typename T> std::string to_string(const T& n)
{
	return std::to_string(n);
}
#endif

}

namespace parche_mingw
{

#ifdef WINCOMPIL
template <typename T> std::string to_string(const T& n)
{	
	return compat::to_string(n);
}
#endif
#ifndef WINCOMPIL
template <typename T> std::string to_string(const T& n)
{
	return compat::to_string(n);	
}
#endif

}

#endif
