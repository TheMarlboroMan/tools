#include "i8n.h"

#include <algorithm>
#include <ctype.h>

#include "../source/string_utils.h"
#include "../templates/algorithm.h"

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
		+" ending at char "
		+std::to_string(_char)) {

}

tools::i8n_parser_error::i8n_parser_error(const std::string _err)
	:i8n_exception("parser error in "+_err) {

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

	//TODO: I hate this in the constructor...
	for(const auto& pair : _lexer_tokens) {
		try {
			parse_file(pair.second);
		}
		catch(i8n_parser_token_error& e) {
			throw i8n_parser_error(pair.first+" : "+e.what());
		}
	}

	for(const auto& pair : entries) {
		std::cout<<" >>> "<<pair.first<<std::endl;
		debug_entry(pair.second, std::cout);
	}

	//Scan every embed, see if we have incomplete definitions..
	check_integrity();

	//Compile them so every embed dissapears and only literals and variables remain.
	std::map<std::string, codex_entry> solved;
	int last_solved=0;
	while(true) {

		//Move solvable entries around...
		for(auto it=std::begin(entries); it != std::end(entries);) {

			if(is_entry_solved(it->second, solved)) {
				solved[it->first]=it->second;
				it=entries.erase(it);
			}
			else if(solve_entry(it->second, solved)) {
				solved[it->first]=it->second;
				it=entries.erase(it);
			}
			else {
				it++;
			}
		}

		std::cout<<"PASS----------------------------"<<std::endl;
		for(const auto& p : solved) {
			std::cout<<"solved "<<p.first<<std::endl;
		}

		//Nothing was solved in this pass: circular references!!!
		if(last_solved==solved.size()) {

			//TODO: Add list of unsolvable to the error...

			throw i8n_parser_error("circular references found in data");
		}

		last_solved=solved.size();
	}

	//TODO: Compact consecutive literals into one.
}

void tools::i8n::parser::debug_tokens(const std::vector<lexer::token>& _tokens, std::ostream& _stream) {

	for(const auto& token : _tokens) {
		debug_token(token, _stream);
	}
}

void tools::i8n::parser::debug_token(const lexer::token& _token, std::ostream& _stream) {

	switch(_token.type) {
		case lexer::tokentypes::openlabel: 	_stream<<"[OPENLAB] "<<_token.line<<":"<<_token.charnum<<std::endl; break;
		case lexer::tokentypes::closelabel: _stream<<"[CLSELAB] "<<_token.line<<":"<<_token.charnum<<std::endl; break;
		case lexer::tokentypes::openvalue: 	_stream<<"[OPENVAL] "<<_token.line<<":"<<_token.charnum<<std::endl; break;
		case lexer::tokentypes::closevalue:	_stream<<"[CLSEVAL] "<<_token.line<<":"<<_token.charnum<<std::endl; break;
		case lexer::tokentypes::openvar:	_stream<<"[OPENVAR] "<<_token.line<<":"<<_token.charnum<<std::endl; break;
		case lexer::tokentypes::closevar:	_stream<<"[CLSEVAR] "<<_token.line<<":"<<_token.charnum<<std::endl; break;
		case lexer::tokentypes::openembed:	_stream<<"[OPENEMB] "<<_token.line<<":"<<_token.charnum<<std::endl; break;
		case lexer::tokentypes::closeembed:	_stream<<"[CLSEEMB] "<<_token.line<<":"<<_token.charnum<<std::endl; break;
		case lexer::tokentypes::nothing:	_stream<<"[NOTHING] "<<_token.line<<":"<<_token.charnum<<std::endl; break;
		case lexer::tokentypes::literal:	_stream<<"[LITERAL] "<<_token.line<<":"<<_token.charnum<<" '"<<_token.val<<"'"<<std::endl; break;
	}
}

void tools::i8n::parser::debug_entry(const codex_entry& _entry, std::ostream& _stream) {

	for(const auto& s : _entry.segments) {
		debug_segment(s, _stream);
	}
}

void tools::i8n::parser::debug_segment(const entry_segment& _segment, std::ostream& _stream) {

	switch(_segment.type) {
		case entry_segment::types::literal: 	_stream<<"[LIT] "<<_segment.value<<std::endl; break;
		case entry_segment::types::variable:	_stream<<"[VAR] "<<_segment.value<<std::endl; break;
		case entry_segment::types::embed: 		_stream<<"[EMB] "<<_segment.value<<std::endl; break;
	}
}

void tools::i8n::parser::parse_file(const std::vector<lexer::token>& _tokens) {

	int curtoken=0,
		size=size=_tokens.size()-1;

	//Parse the tokens...
	while(true) {
		if(!label_phase(_tokens, curtoken, size)) {
			break;
		}
		value_phase(_tokens, curtoken, size);
	}
}

void tools::i8n::parser::check_integrity(){

	typedef const std::pair<std::string, codex_entry> tpair;

	size_t total=tools::reduce(std::begin(entries), std::end(entries), [](tpair _pair) {return _pair.second.segments.size();}, 0);
	std::vector<entry_segment> embeds(total);

	for(const auto& pair : entries) {
		std::copy_if(std::begin(pair.second.segments), std::end(pair.second.segments), std::begin(embeds), [this](const entry_segment& _seg) {
			return entry_segment::types::embed==_seg.type
				&& !entries.count(_seg.value);
		});
	}
	
	if(embeds.size()) {
		std::string list;
		std::for_each(std::begin(embeds), std::end(embeds), [&list](const entry_segment& _seg) {
			list+=_seg.value+",";
		});
		
		list.pop_back();
		throw i8n_parser_error("undefined references found in data : "+list);
	}
}


bool tools::i8n::parser::is_entry_solved(const codex_entry& _entry, const std::map<std::string, codex_entry>& _solved) const {

	//TODO: 

	//No embeds equals instantly solvable. Unsolved embeds must be left for now...
	for(const auto& segment : _entry.segments) {
		if(entry_segment::types::embed==segment.type && !_solved.count(segment.value)) {
			return false;
		}
	}

	return true;
}

bool tools::i8n::parser::solve_entry(codex_entry& _entry, const std::map<std::string, codex_entry>& _solved) {

	for(auto it=std::begin(_entry.segments); it != std::end(_entry.segments); it++) {

		auto& segment=*it;
		if(entry_segment::types::embed==segment.type) {
			if(!_solved.count(segment.value)) {
				return false;
			}

			//Remove the embed entry. Step back to insert the new stuff...
			it=_entry.segments.erase(it);
			if(std::begin(_entry.segments)!=it) {
				--it;
			}

			//Add the new ones... Reset: the new segments may include embeds too!
			const auto& new_segments=_solved.at(segment.value).segments;
			_entry.segments.insert(it, std::begin(new_segments), std::end(new_segments));
			it=std::begin(_entry.segments);
		}
	}

	return true;
}

bool tools::i8n::parser::label_phase(const std::vector<lexer::token>& _tokens, int& _curtoken, const int _size) {

	//Trim all whitespace tokens before the first opening...
	_curtoken=find_next_of(_tokens, lexer::tokentypes::openlabel, _curtoken);

	//Either there are not tokens or there's only whitespace.
	if(_curtoken > _size) {
		return false;
	}

	parse_open_close(_tokens, lexer::tokentypes::closelabel, _curtoken, &parser::create_entry);
	_curtoken+=3;

	return true;
}

void tools::i8n::parser::value_phase(const std::vector<lexer::token>& _tokens, int& _curtoken, const int _size) {

	//Store these, we are skipping to the "value open" now and might get to the end of the tokens.
	int line=_tokens[_curtoken].line,
		charnum=_tokens[_curtoken].charnum;

	_curtoken=find_next_of(_tokens, lexer::tokentypes::openvalue, _curtoken);
	if(_curtoken > _size) {
		throw i8n_parser_token_error("expecting value open", line, charnum);
	}

	//Curtoken is sits now at the beginning of the next token, if any...
	++_curtoken;

	//Now, read until we find a "close value...""
	while(true){

		//If there are no more tokens, the syntax of the file was not ok...
		if(_curtoken > _size) {
			throw i8n_parser_token_error("value not closed after ", line, charnum);
		}

		const auto& tok=_tokens[_curtoken];
		auto curtype=tok.type;

		//We are looking for literals, openvar or openembed...
		switch(curtype) {
			case lexer::tokentypes::literal:
				entries[current_label].segments.push_back({entry_segment::types::literal, tok.val});
			break;
			case lexer::tokentypes::openvar:
				parse_open_close(_tokens, lexer::tokentypes::closevar, _curtoken, &parser::add_var);
				_curtoken+=2;
			break;
			case lexer::tokentypes::openembed:
				parse_open_close(_tokens, lexer::tokentypes::closeembed, _curtoken, &parser::add_embed);
				_curtoken+=2;
			break;
			case lexer::tokentypes::closevalue:
				++_curtoken;
				return;
			break;
			default:
				throw i8n_parser_token_error("unexpected '"+lexer::typetostring(curtype)+"' inside value", tok.line, tok.charnum);
		}

		++_curtoken;
	}
}

void tools::i8n::parser::parse_open_close(const std::vector<lexer::token>& _tokens, lexer::tokentypes _closetype, int _curtoken, void(parser::*_callback)(const lexer::token&)) {

	//Skip the opening...
	++_curtoken;

	//Check we have a literal...
	if(lexer::tokentypes::literal!=_tokens[_curtoken].type) {
		throw i8n_parser_token_error("unexpected '"+lexer::typetostring(_tokens[_curtoken].type)+"', expecting literal", _tokens[_curtoken].line, _tokens[_curtoken].charnum);
	}

	(this->*_callback)(_tokens[_curtoken]);

	//Skip the value and check we are closing...
	++_curtoken;
	if(_closetype!=_tokens[_curtoken].type) {
		throw i8n_parser_token_error("unexpected '"+lexer::typetostring(_tokens[_curtoken].type)+"', expecting '"+lexer::typetostring(_closetype)+"'", _tokens[_curtoken].line, _tokens[_curtoken].charnum);
	}
}

void tools::i8n::parser::create_entry(const lexer::token& _tok) {

	//Check we have an unique id.
	const std::string val=_tok.val;

	if(entries.count(val)) {
		throw i8n_parser_token_error("repeated entry identificator", _tok.line, _tok.charnum);
	}

	current_label=val;
	entries[current_label]=codex_entry{};
}

void tools::i8n::parser::add_embed(const lexer::token& _tok) {

	entries[current_label].segments.push_back({entry_segment::types::embed, _tok.val});
}

void tools::i8n::parser::add_var(const lexer::token& _tok) {

	entries[current_label].segments.push_back({entry_segment::types::variable, _tok.val});
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

			throw i8n_parser_token_error("unskippable "+lexer::typetostring(tok.type)+", expecting '"+lexer::typetostring(_type)+"'", tok.line, tok.charnum);
		}

		throw i8n_parser_token_error("unexpected token, expecting '"+lexer::typetostring(_type)+"', found '"+lexer::typetostring(tok.type)+"'", tok.line, tok.charnum);
	}

	return _curtoken;
}
