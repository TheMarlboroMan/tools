#ifndef TOOLS_STRING_UTILS_H
#define TOOLS_STRING_UTILS_H

#include <string>
#include <vector>
#include <map>

namespace tools
{

//!Creates a string from the vector, separated by the second parameter.
std::string					implode(const std::vector<std::string>&, const char);

//!Creates a string from the vector, separated by the second parameter.
std::string					implode(const std::vector<std::string>&, const std::string&);

//!Splits a string by a given char and returns it in a vector of size as large
//!as the "max" parameter. If max is zero the length is unbound.
std::vector<std::string> 		explode(const std::string &, const char, size_t max=0);

//!Splits a string by a given string and returns it in a vector of size as large
//!as the "max" parameter. If max is zero the length is unbound.
std::vector<std::string> 		explode(const std::string &, const std::string&, size_t max=0);

//!In the string given as the first argument, replaces each occurrence of the
//!second argument with the third. The subject string will be altered and returned.
std::string&				replace(std::string&, const std::string&, const std::string&);

//!Trims the string of whitespace characters from the left, changing and returning it.
std::string& 				ltrim(std::string&);
//!Trims the string of whitespace characters from the right, changing and returning it.
std::string& 				rtrim(std::string&);
//!Trims the string of whitespace characters, changing and returning it.
std::string& 				trim(std::string&);

//!Returns a new string, trimmed of whitespace from the left.
std::string 				str_ltrim(const std::string&);

//!Returns a new string, trimmed of whitespace from the right.
std::string 				str_rtrim(const std::string&);

//!Returns a new string, trimmed of whitespace.
std::string 				str_trim(const std::string&);

//!Returns a new string following the "replace" prototype.
std::string				str_replace(const std::string&, const std::string&, const std::string&);

//!Returns the number of octets that should follow the beginning of a UTF8 string.
unsigned short int 			utf8_begin_bytes(const char);

//!Checks if the UTF8 mark corresponds to a 2-octet character.
bool 					is_utf8_begin_2b(const char);

//!Checks if the UTF8 mark corresponds to a 3-octet character.
bool 					is_utf8_begin_3b(const char);

//!Checks if the UTF8 mark corresponds to a 4-octet character.
bool 					is_utf8_begin_4b(const char);

//!Checks if the UTF8 mark corresponds to a 5-octet character.
bool 					is_utf8_begin_5b(const char);

//!Checks if the UTF8 mark corresponds to a 6-octet character.
bool 					is_utf8_begin_6b(const char);

//!Converts the two octets of a 2-bit UTF8 character to a wchar.
unsigned char 				utf8_2b_to_uchar(char, char);

//!Converts a wchar string to its char representation.
char * 					wstring_to_8b(const wchar_t *);

//!Returns a string resulting of splitting the given one into many lines of pmax length.
std::string 				split_to_lines(const std::string&, unsigned int);

//!Returns a string with the binary representation (with 0 and 1) of the given char.
std::string 				as_binary(char);

//!Returns a string with the binary representation (with 0 and 1) of the given string.
std::string 				as_binary(const std::string&);

//!Calculates the two's complement of the given char (invert bytes).
unsigned char 				twos_complement(char);

}

#endif
