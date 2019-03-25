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

tools::i8n_lexer_error::i8n_lexer_error(const std::string& _err, const std::string& _file, const std::string& _line, int _linenum)
	:i8n_exception("lexer error: "
		+_err
		+" in file "
		+_file
		+" line ["
		+std::to_string(_linenum)
		+"] '"
		+_line
		+"'") {
};

tools::i8n_lexer_generic_error::i8n_lexer_generic_error(const std::string& _err)
	:std::runtime_error(_err) {

};


tools::i8n_parser_token_error::i8n_parser_token_error(const std::string _err, int _line, int _char)
	:i8n_exception(_err
		+" in line "
		+std::to_string(_line)
		+" at char "
		+std::to_string(_char)) {

}

////////////////////////////////////////////////////////////////////////////////
// Main

//!Class constructor with path and default language.
tools::i8n::i8n(const std::string& _path, const std::string& _lan, const std::vector<std::string>& _input)
	:file_path(_path), language(_lan) {

	//TODO: Exception handling: should not leak to the outside anything other than the main type.
	for(const auto& _i : _input) {
		add_private(_i);
	}

	build_entries();
}

void tools::i8n::add(const std::string& _input) {

	//TODO: Exception handling: should not leak to the outside anything other than the main type.
	add_private(_input);
	build_entries();
}

void tools::i8n::add_private(const std::string& _path) {

	if(!_path.size()) {
		throw i8n_exception_no_path{};
	}

	if(!language.size()) {
		throw i8n_exception_no_language{};
	}

	const std::string path=file_path+"/"+language+"/"+_path;
	std::ifstream file(path);

	if(!file) {
		throw i8n_exception_file_error(path);
	}

	lexer lx;

	//TODO: Exception handling: should not leak to the outside anything other than the main type.

	//TODO: Store the tokens with the file...
	lexer_tokens[_path]=lx.process(path);

/*
	if(codex.count(_input.key)) {
		throw i8n_exception_repeated_key(_input.key);
	}
*/
//TODO
//	codex[_input.key]=codex_page{_input.path};
//	parser parser(_codex[__input.key], file);
//	parser.parse();
}

void tools::i8n::set(const substitution& _subs) {

	substitutions[_subs.key]=_subs.value;
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

void tools::i8n::build_entries() {

	//TODO: Exceptions and shizzz.
	parser pr(lexer_tokens);
	lexer_tokens.clear();
}

void tools::i8n::reload_codex() {

/*
TODO

	//!Clear current entries, keep the path.
	for(auto& e : codex) {
		e.second.clear();
	}

	//!Reload.
	for(auto& e : codex) {
		add(e.first, e.second.path);
	}
*/
}

////////////////////////////////////////////////////////////////////////////////
// Lexer.

std::vector<tools::i8n::lexer::token> tools::i8n::lexer::process(const std::string& _filepath) {

	std::string line;
	int linenum=0, charnum=0;

	try {
		std::ifstream file(_filepath.c_str());
		if(!file) {
			throw i8n_lexer_generic_error("cannot open file");
		}

		//The lexer just reads line by line, storing data as tokens are found.
		std::string buffer;
		std::vector<tools::i8n::lexer::token>	result;

		while(true) {
			std::getline(file, line);
			++linenum;
			charnum=0;

			if(file.eof()) {
				break;
			}

			//Skip comments... Blank lines will not be skipped, as they might carry meaning!
			if(comment==line.front()) {
				continue;
			}

			line+=tools::newline; //Newlines are lost in getline, but they might carry significance here...
			while(line.size()) {

				++charnum;
				buffer+=line.front();
				line.erase(0, 1);

				auto size=buffer.size();
				if(size >= 2) {
					auto last_two=buffer.substr(size-2);
					auto type=scan_buffer(last_two);
					if(tokentypes::nothing!=type) {

						if(size > 2) {
							result.push_back({tokentypes::literal, buffer.substr(0, size-2), linenum, charnum-2});
						}

						result.push_back({type, last_two, linenum, charnum});
						buffer.clear();

						//When closing a value we discard the rest of the line. A little convenience thing.
						if(tokentypes::closevalue==type) {
							line.clear();
						}
					}
				}
			}
		}

		//!The last thing we expect is actually a delimiter, so this is an error.
		if(str_trim(buffer).size()) {
			throw i8n_lexer_generic_error("non-token found at the end of the file");
		}

		return result;
	}
	catch(i8n_lexer_generic_error& e) {
		throw i8n_lexer_error(_filepath, line, e.what(), linenum);
	}
}

tools::i8n::lexer::tokentypes tools::i8n::lexer::scan_buffer(const std::string& _control) {

	//This should actually be an assertion, but hey...
	if(2!=_control.size()) {
		throw i8n_lexer_generic_error("scan buffer cannot be called with a string larger than 2 characters");
	}

	if(open_label==_control) 		return tokentypes::openlabel;
	else if(close_label==_control) 	return tokentypes::closelabel;
	else if(open_value==_control) 	return tokentypes::openvalue;
	else if(close_value==_control)	return tokentypes::closevalue;
	else if(open_var==_control) 	return tokentypes::openvar;
	else if(close_var==_control) 	return tokentypes::closevar;
	else if(open_embed==_control) 	return tokentypes::openembed;
	else if(close_embed==_control) 	return tokentypes::closeembed;
	return tokentypes::nothing;
}

std::string tools::i8n::lexer::typetostring(tokentypes _type) {

	switch(_type) {
		case tokentypes::openlabel: return "open label";
		case tokentypes::closelabel: return "close label";
		case tokentypes::openvalue: return "open value";
		case tokentypes::closevalue: return "close value";
		case tokentypes::openvar: return "open variable";
		case tokentypes::closevar: return "close variable";
		case tokentypes::openembed: return "open embed";
		case tokentypes::closeembed: return "close embed";
		case tokentypes::literal:	return "literal";
		case tokentypes::nothing:	return "nothing";
	}
}


////////////////////////////////////////////////////////////////////////////////
// Parser.

//TODO TODO TODO
#include <iostream>
//TODO TODO TODO
tools::i8n::parser::parser(const std::map<std::string, std::vector<lexer::token>>& _lexer_tokens) {

	//TODO: handle exceptions..

	for(const auto& pair : _lexer_tokens) {

		debug_tokens(pair.second, std::cout);

		//TODO.
		//const std::string filename=pair.first;
		parse_file(pair.second);
	}
}

void tools::i8n::parser::debug_tokens(const std::vector<lexer::token>& _tokens, std::ostream& _stream) {

	for(const auto& token : _tokens) {
		switch(token.type) {
			case lexer::tokentypes::openlabel: 	_stream<<"[OPENLAB]"<<std::endl; break;
			case lexer::tokentypes::closelabel: _stream<<"[CLSELAB]"<<std::endl; break;
			case lexer::tokentypes::openvalue: 	_stream<<"[OPENVAL]"<<std::endl; break;
			case lexer::tokentypes::closevalue:	_stream<<"[CLSEVAL]"<<std::endl; break;
			case lexer::tokentypes::openvar:	_stream<<"[OPENVAR]"<<std::endl; break;
			case lexer::tokentypes::closevar:	_stream<<"[CLSEVAR]"<<std::endl; break;
			case lexer::tokentypes::openembed:	_stream<<"[OPENEMB]"<<std::endl; break;
			case lexer::tokentypes::closeembed:	_stream<<"[CLSEEMB]"<<std::endl; break;
			case lexer::tokentypes::nothing:	_stream<<"[NOTHING]"<<std::endl; break;
			case lexer::tokentypes::literal:	_stream<<"[LITERAL]"<<token.val<<std::endl; break;
		}
	}
}

void tools::i8n::parser::parse_file(const std::vector<lexer::token>& _tokens) {

	//Trim all whitespace tokens before the first opening...
	int curtoken=0,
		size=size=_tokens.size()-1;

	curtoken=find_next_of(_tokens, lexer::tokentypes::openlabel, curtoken);

	//Either there are not tokens or there's only whitespace.
	if(curtoken >= size) {
		return;
	}

	//Skip the opening...
	++curtoken;
	if(lexer::tokentypes::literal!=_tokens[curtoken].type) {
		throw i8n_parser_token_error("unexpected token, expecting literal after label open", _tokens[curtoken].line, _tokens[curtoken].charnum);
	}

	//Check we have an unique id.
	if(entries.count(_tokens[curtoken].val)) {
		throw i8n_parser_token_error("repeated entry identificator", _tokens[curtoken].line, _tokens[curtoken].charnum);
	}

	entries[_tokens[curtoken].val]=codex_entry{};

	//Check we are closing...
	++curtoken;
	if(lexer::tokentypes::closelabel!=_tokens[curtoken].type) {
		throw i8n_parser_token_error("unexpected token, expecting label close", _tokens[curtoken].line, _tokens[curtoken].charnum);
	}

	//Store these, we are skipping to the value now and might get to the end of the tokens.
	int line=_tokens[curtoken].line,
 		charnum=_tokens[curtoken].charnum;

	++curtoken;
	curtoken=find_next_of(_tokens, lexer::tokentypes::openvalue, curtoken);
	if(curtoken >= size) {
		throw i8n_parser_token_error("expecting value open", line, charnum);
	}

	//Skip the opening...
	++curtoken;
	//TODO: All bets are off here!!!.
}

int tools::i8n::parser::find_next_of(const std::vector<lexer::token>& _tokens, lexer::tokentypes _type, int _curtoken) {

	int size=_tokens.size()-1;
	while(_curtoken < size) {

		const auto& tok=_tokens[_curtoken];
		if(_type==tok.type)  {
			return _curtoken;
		}

		if(lexer::tokentypes::literal==tok.type) {
			if(!str_trim(tok.val).size()) {
				++_curtoken;
				continue;
			}

			throw i8n_parser_token_error("unexpected literal ", tok.line, tok.charnum);
		}

		throw i8n_parser_token_error("unexpected token, expecting '"+lexer::typetostring(_type)+"', found '"+lexer::typetostring(tok.type)+"'", tok.line, tok.charnum);
	}

	return _curtoken;
}
