#include "i8n.h"

#include <ctype.h>

#include "../source/string_utils.h"

////////////////////////////////////////////////////////////////////////////////
// Exceptions

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

tools::i8n_exception_parse_error::i8n_exception_parse_error(const std::string& _err)
	:i8n_exception(_err) {

}

tools::i8n_exception_parse_syntax_error::i8n_exception_parse_syntax_error(const std::string& _err, const std::string& _file, const std::string& _line, int _linenum)
	:i8n_exception("parse error: "
		+_error
		+" in file "
		+_file
		+" line ["
		+std::to_string(_linenum)
		+"] '"
		+_line
		+"'")
	,error(_error), file(_file), line(_line), linenum(_linenum) {

}

tools::i8n_lexer_generic_exception::i8n_lexer_generic_exception(const std::string& _err)
	:i8n_exception("lexer error "+_err) {

}

////////////////////////////////////////////////////////////////////////////////
// Main

//!Class constructor with path and default language.
tools::i8n::i8n(const std::string& _path, const std::string& _lan, const std::vector<i8n_input>& _input)
	:file_path(_path), language(_lan) {

	for(const auto& _i : _input) {
		add_private(_i);
	}

	build_entries();
}

void tools::i8n::add(const i8n_input& _input) {

	add_private(_input);
	build_entries();
}

void tools::i8n::add_private(const i8n_input& _input) {

	if(!_input.path.size()) {
		throw i8n_exception_no_path;
	}

	if(!language.size()) {
		throw i8n_exception_no_language;
	}

	const std::string path=file_path+"/"+language+"/"+_input.path;
	std::ifstream file(path);

	if(!file) {
		throw i8n_exception_file_not_found(path);
	}

	if(codex.count(_input.key)) {
		throw i8n_exception_repeated_key(_input.key);
	}

	codex[_input.key]=codex_page{_input.path};

	file_parser parser(_codex[__input.key], file);
	parser.parse();
}

void tools::i8n::set(const std::string& _key, const std::string& _value) {

	substitutions[_key]=_value;
}

void tools::i8n::set_root(const std::string& _path) {

	file_path=_path;
	reload_codex();
	build_entries();
}

void tools::i8n::set_language(const std::string& _lan) {

	language=_lan;
	reload_codex();
	build_entries();
}

std::string tools::i8n::get(const i8n_get& _get) const {

	//TODO.
	return "LALA";
}

std::string tools::i8n::get(const i8n_get& _get, const std::vector<substitution>& _subs) const {

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

////////////////////////////////////////////////////////////////////////////////
// File Parser.

		//TODO: Not really: the parser should keep a list of keys to values
		//and then compile them into a page.
tools::i8n::file_parser::file_parser(codex_page& _page, std::ofstream& _file)
	:page(_page), file(_file) {

}

		//TODO: Not really: the parser should keep a list of keys to values
		//and then compile them into a page.
void tools::i8n::file_parser::parse() {

//TODO:
/*
The parser can keep a list of keys to values to this and to that, but must
also keep that list related to the original file key, so that we can do a first
pass keeping raw symbols and a second pass translating them. Perhaps we should
forget about the part on which different files can contain the same symbols 
and do a single pass, symbol to raw string, and a second pass, symbol to 
segments... Now that I think about it, that second pass might be problematic:
I might try to solve the symbol "A" first, which has a reference to "B", so 
"A" must be put apart until "B" can be solved, which might depend con "C"
which might again depend on "A": boom, circular reference... I say, solve in
order, if a symbol cannot be solved because it depends on another, put it away
and take the next. At the end, start over with the ones that were put away.
Repeat ad nauseam, anytime that there's the same amount of unsolved symbols
between passes, we got circular references and we throw.
*/

//TODO; The whole thing is perverse, actually, let us start with a lexer, 
//that produces a vector of lexicon and later apply the parser to it.

	std::string line;
	int linenum=0;

	try {
		while(true) {

			std::readline(file, line);
			++linenum;

			//Add the new line, since it is valid in this context.
			line+=tools::nl;

			//Reverse, so we can pop from the back...
			std::reverse(std::begin(line), std::end(line));

			if(file.eof()) {
				if(modes::scan!==mode) {
					throw i8n_exception_parse_error("reached en of file in a mode different from 'scan'. Perhaps the last item was not closed?");
				}
			}

			while(line.size()) {

				char current=_line.pop_back();
				buffer+=current;
				control(current);

				switch(mode) {
					case modes::scan: parse_scan(current, line); break;
					case modes::key: parse_key(current, line); break;
					case modes::value: parse_value(current, line); break;
				}
			}
		}
	}
	catch(i8n_exception_parse_error& e) {
		throw i8n_exception_parse_syntax_error(e.what(), page.path, line, linenum);
	}
}

void tools::i8n::file_parser::parse_scan(const char _cur, std::string& _line) {

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

void tools::i8n::file_parser::parse_key(const char _cur, const std::string& _line) {

	//TODO: Actually, we should only allow alnum characters,
	//perhaps some _- and maybe .
	
	//TODO: Should catch new lines. Check it.
	if(std::isspace(_cur)) {
		throw i8n_exception_parse_error("whitespace character found when reading key");
	}

	//Check if we reached the open_entry delimiter. 
	if(control_buffer==open_entry) {

		key=buffer;
		key.pop_back(); //Remove the delimiter...
		key.pop_back();

		if(!key.size()) {
			throw i8n_exception_parse_error("zero length key");
		}

		buffer.clear();
		mode=modes::value;
	}
	
}

void tools::i8n::file_parser::parse_value(const char _cur, std::string& _line) {

	//Check if we reached the close_entry delimiter.
	if(control_buffer==close_entry) {

		buffer.pop_back(); //Remove the delimiter...
		buffer.pop_back()

		if(page.count(key)) {
			throw i8n_exception_parse_error("repeated key");
		}

		page.entries[key]=buffer;
		buffer.clear();
		key.clear();
		line.clear();	//The rest of the line is discarded.
		mode=modes::scan;
	}
}

void tools::i8n::file_parser::control(const char _c) {

	control_buffer+=_c;

	//Always keep 2 chars, as that's the size of the delimiters.
	if(control_buffer.size() > 2) {
		std::reverse(std::begin(control_buffer), std::end(control_buffer));
		control_buffer.pop_back();
	}	
}

////////////////////////////////////////////////////////////////////////////////
// Lexer.

std::vector<tools::i8n::lexer::token> tools::i8n::lexer::process(const std::string& _filepath) {

	try {
		std::ofstream(_filepath.c_str());
		if(!file) {
			throw i8n_lexer_generic_exception("cannot open file");
		}

		//The lexer just reads line by line, storing data as tokens are found.
		std::string buffer;
		std::string line;
		std::vector<tools::i8n::lexer::token>	result;

		while(true) {
			std::readline(file, line);

			if(file.eof()) {
				break;
			}

			//Skip comments... Blank lines will not be skipped, as they might carry meaning!
			if(comment==line.front()) {
				continue;
			}

			//Add the newline, invert so we can pop from the back.
			file+=tools::newline;
			std::reverse(std::begin(line), std::end(line));
			while(line.size()) {

				buffer+=line.back();
				line.pop_back();

				auto size=buffer.size();
				if(size >= 2) {
					bool	with_trailing_data=size > 2;
					auto type=scan_buffer(buffer.substr(size-2));
					if(tokens::nothing!=type) {

//TODO: here.
						if(with_trailing_data) {
							result.push_back({tokentypes::literal, "HERE GOES THE BUFFER MINUS THE LAST TWO");
						}

						result.push_back({type, ""});
						buffer.clear();
					}
				}
			}
		}

		//if the buffer is not empty, add one last token, surely a string.
		if(buffer.size()) {
			result.push_back({tokentypes::literal, "HERE GOES THE BUFFER MINUS THE LAST TWO");
		}

		return result;
	}
	//TODO: Use a better hierarchy of exceptions.
	catch(i8n_lexer_exception& e) {
		throw i8n_lexer_exception("at file "+_filepath+" : "+e.what());
	}
}

tools::i8n::lexer::tokens tools::i8n::lexer::scan_buffer(const std::string& _control) {

	if(open_label==_control) return tokens::openlabel;
	else if(close_label==_control) return tokens::closelabel;
	else if(open_value==_control) return tokens::openvalue;
	else if(close_value==control) return tokens::closevalue;
	else if(open_var==control) return tokens::openvar;
	else if(close_var==control) return tokens::closevar;
	else if(open_embed==control) return tokens::openembed;
	else if(close_embed==control) return tokens::closeembed;
	return tokens::nothing;
}

////////////////////////////////////////////////////////////////////////////////
// Helpers

tools::i8n::codex_page::codex_page(const std::string _file)
	:path(_file) {

}

