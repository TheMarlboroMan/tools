#include "sprite_table.h"
#include "../source/string_utils.h"
#include "../templates/compatibility_patches.h"

using namespace tools;

sprite_table::sprite_table(const std::string& path) {
	load(path);
}

const sprite_frame& sprite_table::get(size_t index) const {

	return data.at(index);
}

sprite_frame sprite_table::get(size_t index) {

	if(data.count(index)) return data[index];
	else return sprite_frame();
}

void sprite_table::load(const std::string& path) {
	text_reader L(path, '#');

	if(!L){
		throw std::runtime_error(std::string("Unable to locate sprite file ")+path);
	}
	else {
		std::string line;
		const char delimiter='\t';
		
		while(true) {
			line=L.read_line();
			if(L.is_eof()) {
				break;
			}

			std::vector<std::string> values=explode(line, delimiter);
			if(values.size()==7) {
				sprite_frame f;
				size_t index=std::atoi(values[0].c_str());
				f.x=std::atoi(values[1].c_str());
				f.y=std::atoi(values[2].c_str());
				f.w=std::atoi(values[3].c_str());
				f.h=std::atoi(values[4].c_str());
				f.disp_x=std::atoi(values[5].c_str());
				f.disp_y=std::atoi(values[6].c_str());
				data[index]=f;
			}
			else {
				throw std::runtime_error(std::string("Malformed sprite line ")+path+std::string(" : ")+compat::to_string(L.get_line_number()));
			}
		}
	}
}
