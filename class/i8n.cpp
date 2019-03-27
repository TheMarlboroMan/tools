#include "i8n.h"

#include <algorithm>
#include <ctype.h>
#include <iostream>			//For the debug methods.
#include <iterator>
#include <cassert>

#include "../source/string_utils.h"
#include "../source/file_utils.h"
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

tools::i8n_exception_file_error::i8n_exception_file_error(const std::string& _path)
	:i8n_exception("could not open file "+_path) {

}

tools::i8n_repeated_path::i8n_repeated_path(const std::string& _path)
	:i8n_exception("repeated path '"+_path+"' cannot be added twice") {

}

tools::i8n_lexer_error_with_file::i8n_lexer_error_with_file(const std::string& _err, const std::string& _file)
	:i8n_exception("lexer error: "
		+_err
		+" in file "
		+_file) {
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

	for(const auto& _i : _input) {
		paths.push_back(_i);
		add_private(_i);
	}

	build_entries();
}

void tools::i8n::add(const std::string& _path) {

	if(!_path.size()) {
		throw i8n_exception_no_path{};
	}

	if(!language.size()) {
		throw i8n_exception_no_language{};
	}

	if(std::end(paths)!=std::find(std::begin(paths), std::end(paths), _path)) {
		throw i8n_repeated_path{_path};
	}

	//This is added in the public interface: the private interface may do its own
	//calls that might interfere with this.
	paths.push_back(_path);

	reload_codex();
}

void tools::i8n::add_private(const std::string& _path) {

	const std::string path=file_path+"/"+language+"/"+_path;
	std::ifstream file(path);

	if(!file) {
		throw i8n_exception_file_error(path);
	}

	lexer lx;
	lexer_tokens[_path]=lx.from_file(path);
}

void tools::i8n::set(const substitution& _sub) {

	auto it=std::find(std::begin(substitutions), std::end(substitutions), _sub);
	if(std::end(substitutions)==it) {
		substitutions.push_back(_sub);
	}
	else {
		it->value=_sub.value;
	}
}

void tools::i8n::set_root(const std::string& _path) {

	file_path=_path;
	reload_codex();
}

void tools::i8n::set_language(const std::string& _lan) {

	language=_lan;
	reload_codex();
}

std::string tools::i8n::get(const std::string& _get) const {

	if(!codex.count(_get)) {
		return fail_string(_get);
	}

	return codex.at(_get).get(substitutions);
}

std::string tools::i8n::get(const std::string& _get, const std::vector<substitution>& _subs) const {

	if(!codex.count(_get)) {
		return fail_string(_get);
	}

	return codex.at(_get).get(_subs, substitutions);
}

std::string tools::i8n::fail_string(const std::string& _get) const {

	return "*** FAIL "+_get+" ***";
}

void tools::i8n::build_entries() {

	parser pr;
	codex=pr.parse(lexer_tokens);
	lexer_tokens.clear();
}

void tools::i8n::reload_codex() {

	codex.clear();
	for(auto& p : paths) {
		add_private(p);
	}

	build_entries();
}

////////////////////////////////////////////////////////////////////////////////
// Lexer.

std::vector<tools::i8n::lexer::token> tools::i8n::lexer::from_file(const std::string& _filepath) {

	std::ifstream file(_filepath.c_str());
	if(!file) {
		throw i8n_lexer_generic_error("cannot open file "+_filepath);
	}

	try {
		return process(tools::dump_file(_filepath));
	}
	catch(i8n_lexer_generic_error& e) {
		throw i8n_lexer_error_with_file(e.what(), _filepath);
	}
}

std::vector<tools::i8n::lexer::token> tools::i8n::lexer::process(const std::string& _raw_text) {

	std::string line;
	int linenum=0, charnum=0;
	auto lines=tools::explode(_raw_text, tools::newline);

	//The lexer just reads line by line, storing data as tokens are found.
	std::string buffer;
	std::vector<tools::i8n::lexer::token>	result;

	while(lines.size()) {
		//Pop from front...
		line=lines.front();
		lines.erase(std::begin(lines));
		++linenum;
		charnum=0;

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
		throw i8n_lexer_generic_error("non-token found at the end of the stream");
	}

	return result;
}

tools::i8n::lexer::tokentypes tools::i8n::lexer::scan_buffer(const std::string& _control) {

	assert(2==_control.size());

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

std::map<std::string, tools::i8n::codex_entry> tools::i8n::parser::parse(const std::map<std::string, std::vector<lexer::token>>& _lexer_tokens) {

	for(const auto& pair : _lexer_tokens) {
		interpret_tokens(pair.second);
	}

	check_integrity();
	compile_entries();

	for(auto& pair : solved) {
		compact_entry(pair.second);
	}

	return solved;
}

void tools::i8n::parser::compact_entry(codex_entry& _entry) {
#if __GNUC__ <= 4

	//TODO: Think... polyfill or different implementation??

#else
	//TODO: Only C++14
	auto it=std::rbegin(_entry.segments);

	while(true) {

		auto prev=it+1;
		//TODO: Only C++14
		if(prev == std::rend(_entry.segments)) {
			break;
		}

		if(entry_segment::types::literal==it->type && it->type==prev->type) {

			prev->value+=it->value;
			_entry.segments.erase(it.base()-1); //For a reverse iterator, base is the next element the forward iterator would be pointing to.
			it=prev;
			continue;
		}

		++it;
	}
#endif
}

void tools::i8n::parser::debug(const std::vector<lexer::token>& _tokens, std::ostream& _stream) {

	for(const auto& token : _tokens) {
		debug(token, _stream);
	}
}

void tools::i8n::parser::debug(const lexer::token& _token, std::ostream& _stream) {

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

void tools::i8n::parser::compile_entries() {

	int last_solved=0;
	while(entries.size()) {

		//Move solvable entries around...
		for(auto it=std::begin(entries); it != std::end(entries);) {

			if(solve_entry(it->second)) {
				solved[it->first]=it->second;
				it=entries.erase(it);
			}
			else {
				it++;
			}
		}

		//Nothing was solved in this pass: circular references!!!
		if(last_solved==solved.size()) {

			typedef const std::pair<std::string, codex_entry> tpair;
			throw i8n_parser_error(std::string("circular references found in data :")+tools::reduce(
				std::begin(entries),
				std::end(entries),
				[](std::string&& _carry, const tpair& _item){return _carry+=_item.first+" ";},
				"")
			);
		}

		last_solved=solved.size();
	}
}

void tools::i8n::parser::debug(const codex_entry& _entry, std::ostream& _stream) {

	for(const auto& s : _entry.segments) {
		debug(s, _stream);
	}
}

void tools::i8n::parser::debug(const entry_segment& _segment, std::ostream& _stream) {

	switch(_segment.type) {
		case entry_segment::types::literal: 	_stream<<"[LIT] "<<_segment.value<<std::endl; break;
		case entry_segment::types::variable:	_stream<<"[VAR] "<<_segment.value<<std::endl; break;
		case entry_segment::types::embed: 		_stream<<"[EMB] "<<_segment.value<<std::endl; break;
	}
}

void tools::i8n::parser::interpret_tokens(const std::vector<lexer::token>& _tokens) {

	int curtoken=0,
		size=size=_tokens.size()-1;

	while(true) {
		if(!label_phase(_tokens, curtoken, size)) {
			break;
		}
		value_phase(_tokens, curtoken, size);
	}
}

void tools::i8n::parser::check_integrity(){

	typedef const std::pair<std::string, codex_entry> tpair;
	size_t total=tools::reduce(std::begin(entries), std::end(entries), [](size_t&& _carry, tpair _pair) {return _carry+=_pair.second.segments.size();}, 0);
	std::vector<entry_segment> embeds(total);

	for(const auto& pair : entries) {
		std::copy_if(std::begin(pair.second.segments), std::end(pair.second.segments), std::begin(embeds), [this](const entry_segment& _seg) {
			return entry_segment::types::embed==_seg.type
				&& !entries.count(_seg.value);
		});
	}

	if(embeds.size()) {
		std::string list=tools::reduce(std::begin(embeds), std::end(embeds), [](std::string&& _carry, const entry_segment& _seg) {_carry+=_seg.value+",";}, "");
		list.pop_back(); //Remove last comma.
		throw i8n_parser_error("undefined references found in data : "+list);
	}
}


bool tools::i8n::parser::solve_entry(codex_entry& _entry) {

	for(auto it=std::begin(_entry.segments); it != std::end(_entry.segments); it++) {

		auto& segment=*it;

		if(entry_segment::types::embed==segment.type) {

			const std::string key=segment.value;

			if(!solved.count(key)) {
				return false;
			}

			//Remove the embed entry. Step back to insert the new stuff...
			it=_entry.segments.erase(it);

			//Add the new ones... Reset: the new segments may include embeds too!
			const auto& new_segments=solved.at(key).segments;
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

////////////////////////////////////////////////////////////////////////////////
// Codex entry.

std::string tools::i8n::codex_entry::get(const std::vector<substitution>& _subs) const {

	std::string result;
	for(const auto& seg : segments) {

		if(entry_segment::types::literal==seg.type) {
			result+=seg.value;
		}
		else if(entry_segment::types::variable==seg.type) {
			substitute(seg.value, _subs, result);
		}
	}

	return result;
}

std::string tools::i8n::codex_entry::get(const std::vector<substitution>& _subs, const std::vector<substitution>& _base_subs) const {

	std::string result;
	for(const auto& seg : segments) {

		if(entry_segment::types::literal==seg.type) {
			result+=seg.value;
		}
		else if(entry_segment::types::variable==seg.type) {
			if(!substitute(seg.value, _subs, result)) {
				substitute(seg.value, _base_subs, result);
			}
		}
	}

	return result;
}

bool tools::i8n::codex_entry::substitute(const std::string& _key, const std::vector<substitution>& _subs, std::string& _res) const {

	const auto it=std::find_if(std::begin(_subs), std::end(_subs), [&_key](const substitution& _sub) {
		return _sub.key==_key;
	});

	if(std::end(_subs)!=it) {
		_res+=it->value;
		return true;
	}

	return false;
}

////////////////////////////////////////////////////////////////////////////////
// Helpers.

bool tools::i8n::substitution::operator==(const substitution& _o) const {

	return _o.key==key
		&& _o.value==value;
}
