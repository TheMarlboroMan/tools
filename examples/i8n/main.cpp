#include "../../class/i8n.cpp"
#include "../../source/string_utils.cpp"

int main(int, char **) {

	using namespace tools;

	i8n localization={"data", "en", {"test01.dat", "test02.dat"}};
	//i8n localization={"data", "en", {"test03.dat"}};

	//TODO: This makes the world fail!!!
	localization.add({"test03.dat"});

	localization.set({"var", "supervar"});
	std::cout<<localization.get("label-1")<<std::endl;
	std::cout<<localization.get("label-4")<<std::endl;
	std::cout<<localization.get("complex", {{"varhere","varhere1"}, {"varthere","varthere1"}})<<std::endl;

	return 0;
}
