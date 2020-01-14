#include <tools/i8n.cpp>
#include <tools/string_utils.cpp>
#include <tools/file_utils.cpp>

int main(int, char **) {

	using namespace tools;

	i8n localization={"data", "en", {"test01.dat", "test02.dat"}};
	//i8n localization={"data", "en", {"test03.dat"}};

	localization.add_file({"test03.dat"});

	localization.set({"var", "supervar"});
	std::cout<<localization.get("label-1")<<std::endl;
	std::cout<<localization.get("label-4")<<std::endl;
	std::cout<<localization.get("complex", {{"varhere","varhere1"}, {"varthere","varthere1"}})<<std::endl;
	std::cout<<localization.get("label-doesnotexist")<<std::endl;

//	localization.set_fail_entry("{{Will not be able to find ((__key__))}}");
//	std::cout<<localization.get("label-doesnotexist")<<std::endl;

	return 0;
}
