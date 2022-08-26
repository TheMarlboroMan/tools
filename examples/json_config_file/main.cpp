#include <iostream>
#include <fstream>
#include <stdexcept>
#include <tools/file_utils.h>
#include <tools/json_config_file.h>

int main(int /*argc*/, char ** /*argv*/) {

	try {
		tools::json_config_file cf("nothing.json");
	}
	catch(std::runtime_error& e) {
		//Noop, this is expected...
	}

	try {
		const std::string path="in.json";
		const std::string json_data="{\
	\"nested_int\": {\"integer\": 999},\
	\"int\": 1,\
	\"float\": 1.0,\
	\"double\": 1.0,\
	\"string\":\"this is a string\",\
	\"array\": [1,2,3,4],\
	\"object\": {\"key\":\"value\", \"other\": 33},\
	\"nesting\":{\
		\"this\":{\
			\"is\":{\
				\"nested\":99\
			}\
		}\
	}\
}";
		std::ofstream json_file(path);
		json_file<<json_data<<std::endl;
		json_file.close();
		tools::json_config_file cf("in.json");

		auto test=[&cf]() {

			std::cout<<"testing for nested int:"<<cf.int_from_path("nested_int:integer")<<std::endl;
			std::cout<<"testing for deeply nested integer:"<<cf.int_from_path("nesting:this:is:nested")<<std::endl;
			std::cout<<"testing for int:"<<cf.int_from_path("int")<<std::endl;
			std::cout<<"testing for float:"<<cf.float_from_path("float")<<std::endl;
			std::cout<<"testing for double:"<<cf.double_from_path("double")<<std::endl;
			std::cout<<"testing for string:"<<cf.string_from_path("string")<<std::endl;

			std::cout<<"testing for object:";
			const auto& object=cf.token_from_path("object");
			std::cout<<object["key"].GetString()<<" and "<<object["other"].GetInt()<<std::endl;

			std::cout<<"testing for array:";
			const auto& array=cf.token_from_path("array");
			for(const auto& val : array.GetArray()) {
				std::cout<<val.GetInt()<<" ";
			}
			std::cout<<std::endl;
		};

		test();

		//Setting values...
		std::cout<<"Values will be changed now..."<<std::endl;
		cf.set("nested_int:integer", 12);
		cf.set("nesting:this:is:nested", 13);
		cf.set("int", 14);
		cf.set("float", 15.f);
		cf.set("double", 16.0);
		cf.set("string", "changed");
		cf.set_vector("array", std::vector<int>{5,6,7,8});
		test();

		std::cout<<"Data will be reloaded..."<<std::endl;
		cf.reload();
		test();

		//Setting values...
		std::cout<<"Values will be changed again and saved..."<<std::endl;
		cf.set("nested_int:integer", 12);
		cf.set("nesting:this:is:nested", 13);
		cf.set("int", 14);
		cf.set("float", 15.f);
		cf.set("double", 16.0);
		cf.set("string", "changed");
		cf.set_vector("array", std::vector<int>{5,6,7,8});
		cf.save();
		test();

		//checking for existence of values...
		std::cout<<"testing for not existing path"<<std::endl;
		if(cf.has_path("not_really_a_path")) {

			throw std::runtime_error("not_really_a_path is not as path and should not exist");
		}

		std::cout<<"testing for not existing nested path"<<std::endl;
		if(cf.has_path("not:really:a:path")) {

			throw std::runtime_error("not:really:a:path is not as path and should not exist");
		}

		std::cout<<"testing for existing nested path"<<std::endl;
		if(!cf.has_path("nested_int:integer")) {

			throw std::runtime_error("nested_int:integer should be found");
		}

		std::cout<<"testing for existing path"<<std::endl;
		if(!cf.has_path("string")) {

			throw std::runtime_error("string should be found");
		}

		std::cout<<"New values will be added and saved under a new path..."<<std::endl;
		cf.add("a_new_path", "new");
		cf.add("nested_int:another", 123);
		cf.add("a:new:path", 456);
		cf.set_filepath("config.copy.json");
		cf.save();

		std::cout<<"Testing for the new paths..."<<std::endl;
		if(!cf.has_path("a_new_path")) {

			throw std::runtime_error("a_new_path should be found");
		}

		if(!cf.has_path("nested_int:another")) {

			throw std::runtime_error("nested_int:another should be found");
		}

		if(!cf.has_path("a:new:path")) {

			throw std::runtime_error("a:new:path should be found");
		}

		std::cout<<"will test if adding existing values fail..."<<std::endl;
		try {

			cf.add("a_new_path", "new");
			std::cout<<"error: should not allow an existing key to be added!"<<std::endl;
		}
		catch(std::exception &e) {

			std::cout<<"ok"<<std::endl;
		}

		//cleanup...
		tools::filesystem::remove("config.copy.json");

		return 0;
	}
	catch(std::exception& e) {
		std::cerr<<"error: "<<e.what()<<std::endl;
		return 1;
	}
}
