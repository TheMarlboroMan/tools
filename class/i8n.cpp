#include "i8n.h"

#include <ctype.h>

#include "../source/string_utils.h"

tools::i8n_exception::i8n_exception(const std::string& _err)
	:std::runtime_error(_err) {

}

tools::i8n_exception_no_path::i8n_exception_no_path()
	:i8n_exception("cannot load i8n : no path specified") {

}

tools::i8n_exception_no_language::i8n_exception_no_language()
	:i8n_exception("cannot load i8n : no language specified") {

}

tools::i8n_exception_no_data::i8n_exception_no_data()
	:i8n_exception("the i8n class was not loaded") {

}

tools::i8n_exception_file_error::i8n_exception_file_error(const std::string& _path)
	:i8n_exception("file error on "+_path) {

}

tools::i8n_exception_repeated_key::i8n_exception_repeated_key(const std::string& _key)
	:i8n_exception("repeated i8n key "+_key+" cannot be added twice"){

}

tools::i8n::codex_entry::codex_entry(const std::string _file)
	:path(_file) {

}

tools::i8n::i8n() {

}

//!Class constructor with path and default language.
tools::i8n::i8n(const std::string& _path, const std::string& _lan)
	:file_path(_path), language(_lan) {

}

void tools::i8n::add(const t_filekey& _key, const std::string& _filename) {

	if(!file_path.size()) {
		throw i8n_exception_no_path;
	}

	if(!language.size()) {
		throw i8n_exception_no_language;
	}

	const std::string path=file_path+"/"+language+"/"+_filename;
	std::ifstream file(path);

	if(!file) {
		throw i8n_exception_file_not_found(path);
	}

	if(codex.count(_key)) {
		throw i8n_exception_repeated_key(_key);
	}

	codex[_key]=codex_entry{_filename};

	i8n_parser parser(_codex[_key], file);
	parser.parse();
}

void tools::i8n::set(const std::string& _key, const std::string& _value) {

	substitutions[_key]=_value;
}

void tools::i8n::set_root(const std::string& _path) {

	file_path=_path;
	reload_codex();
}

void tools::i8n::set_language(const std::string& _lan) {

	language=_lan;
	reload_codex();
}

std::string tools::i8n::get(const t_filekey& _key, const t_entrykey& _index) const {

	//TODO.
	return "LALA";
}

std::string tools::i8n::get(const t_filekey& _key, const t_entrykey& _index, const std::vector<substitution>& _subs) const {

	//TODO.
	return "NONO";
}

void tools::i8n::reload_codex() {

	//!Clear current entries, keep the path.
	for(auto& e : codex) {
		e.second.clear();
	}

	//!Reload.
	for(auto& e : codex) {
		add(e.first, e.second.path);
	}
}

tools::i8n::parser::parser(codex_entry& _entry, std::ofstream& _file)
	:entry(_entry), file(_file) {

}

void tools::i8n::parser::parse() {

	std::string line;

	while(true) {
	
		std::readline(file, line);
		//TODO: Reverse line so we can pop from the back.
		//TODO: Add system dependent NL.

		if(file.eof()) {

			if(modes::scan!==mode) {
				//TODO: Fucking syntax error.
			}
		}

		while(line.size()) {

			char current=_line.pop_back();
			buffer+=current;
			control(current);

			switch(mode) {
				case modes::scan: parse_scan(current, line); break;
				case modes::key: parse_key(current); break;
				case modes::value: parse_value(current); break;
			}
		}
	}
}

void tools::i8n::parser::parse_scan(const char _cur, std::string& _line) {

	//Could be a fallacy unless we are absolutely sure we will only be in
	//scan mode at the beginning of a line.
	if(_cur==comment) {
		_line.clear();
		return;
	}

	tools::trim(line);
	if(!line.size()) {
		return;
	}

	mode=modes::key;
}

void tools::i8n::parser::parse_key(const char _cur) {

	if(std::isspace(_cur)) {
		//TODO: Fucking syntax error.
	}
}

void tools::i8n::parser::parse_value(const char _cur) {

}

void tools::i8n::parser::control(const char _c) {

	control_buffer+=_c;
	//TODO: keep the control buffer at bay...
}
