#include <tools/i8n.h>
#include <tools/string_utils.h>
#include <tools/file_utils.h>

#include <iostream>

int main(int, char **) {

	using namespace tools;

	i8n localization={"../examples/i8n/data", "en", {"test01.dat", "test02.dat"}};

	localization.add_file({"test03.dat"});

	localization.set({"var", "supervar"});
	std::cout<<localization.get("label-1")<<std::endl;
	std::cout<<localization.get("label-4")<<std::endl;
	std::cout<<localization.get("complex", {{"varhere","varhere1"}, {"varthere","varthere1"}})<<std::endl;
	std::cout<<localization.get("label-doesnotexist")<<std::endl;

	localization.set_fail_entry("{{Will not be able to find ((__key__))}}");
	std::cout<<localization.get("label-doesnotexist")<<std::endl;

	return 0;
}
