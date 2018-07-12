#ifndef TOOLS_DNOT_PARSER_H
#define TOOLS_DNOT_PARSER_H

#include <cctype>
#include <istream>

#include <fstream>
#include <iostream>

#include <stdexcept>
#include <algorithm>
#include "dnot_token.h"

namespace tools
{

//!A parser for DNOT files.

//!DNOT is a quasy-json format, with a few - but important - differences. DNOT
//!files start with 1-N named root nodes (I have been meaning to change it 
//!for a while now), the insertion order into associative maps is not preserved
//!(it is built upon std::map), keys are not to be enclosed between quotes...
//!It is - still - practical, easy to read and easy enough to parse as to 
//!preserve its place in this toolset. Just don't be too hard on it because it
//!is not json. If you need json, go get a json parser.

class dnot_parser {
	private:

	//!Defines the type of a token and how the parser behaves.
	enum class types {tundefined, tmap, tvector};

	public:

	//!Creates a parser from a stream, saving a line and column for error
	//!reporting purposes (parsers are created recursively). This method
	//!is not designed for public consumption: users are recommended to
	//!make use of any of the free functions in this same file.
						dnot_parser(std::istream&, int=0, int=0);

	//!In a surprising twist, starts the parser. Users will have a better
	//!experiencing using any of the free functions defined in this file.
	void 					operator()();

	//TODO: Un problema con el parser es que los ficheros no tienen un nodo
	//"raiz". Asumiremos siempre que la base es un tobject mientras no pensemos
	//en una solución.

	//!Returns the root token for the parser (fixed as a map). Will throw
	//!if the parser was not started. Any of the free functions defined
	//!in this file will provide the user with a better experience.
	const dnot_token&			get_token() const {
		if(!done) throw std::runtime_error("dnot parser was not executed");
		return token;
	}

	//!Returns the root token for the parser (fixed as a map). Will throw
	//!if the parser was not started. Any of the free functions defined
	//!in this file will provide the user with a better experience.
	dnot_token&				get_token() {
		if(!done) throw std::runtime_error("dnot parser was not executed");
		return token;
	}

	private:
 
	
	//!Reads stream, ignoring whitespace if we are not quoting
	char 					read_stream(bool);

	//!Reads the given character with the parser operating in string mode.
	//!If also exits string mode if double quotes are found.
	void 					process_string(char cb);

	//!Reads the given character in stream mode, which warrants special
	//!characters (,"{}[]).structural meaning.
	void 					process_stream(char cb);

	//!Toggles stream-string state.
	void 					quotes();

	//!Executes the logic after a comma character is found (add something
	//!to a vector o map.
	void 					comma();

	//!Creates a map subparser and processes new tokens.
	void 					open_brace();

	//!Called from a map subparser, assigns the last value and marks it as "done".
	void 					close_brace();

	//!Creates a vector subparser and processes new tokens.
	void 					open_bracket();

	//!Called from a vector subparser, closes it.
	void 					close_bracket();

	//!Assigns a key-value pair to a map token.
	void 					assign_tobject();

	//!Infers a token type from the string value and returns a new token with 
	//!the type and value.
	dnot_token 				generate_token(const std::string& v);

	//!Called once a comma is found in a vector parser, assigns the streamed
	//!element to the current token.
	void 					assign_tarray();

	//!Assigns the map parameter to the current parser as an object.
	void 					assign_tobject_subparser(const std::map<std::string, dnot_token>& aux);

	//!Assigns the map parameter to the current parser as an array.
	void 					assign_tarray_subparser(const std::vector<dnot_token>& aux);

	//!Throws an exception with the given message.
	void 					error(const std::string& msj);

	//!Clears the stream buffer.
	void					clear_buffer();

	//!Returns a string representation of the parser state, for error 
	//!reporting purposes.
	std::string 				get_state();

	//!Differnt parser states.
	enum class tstates {reading, exiting, exiting_subparser};

	tstates			state;		//!< Current parser state.
	bool			read_quotes, 	//!< Current parser mode (string/stream).
				done,		//!< Indicates that the parser has finished parsing the stream.
				root;		//!< True when the parser is not a subparser.
	types			type;		//!< Current type.
	std::istream&		stream; 	//!< Current stream reference, will be passed along subparsers.
	std::string 		buffer;		//!< Internal data buffer.

	int			line, 		//!< Current line in the stream.
				column;		//!< Current column in the stream.

	dnot_token				token; //!< Base token for this parser. Subparsers return tokens that are bound to this one.

};

//!Alias of dnot_parse_file (actually, it is exactly the other way around).
dnot_token	dnot_parse(const std::string&);
//!Parses the given file, returns the dnot root node.
dnot_token	dnot_parse_file(const std::string& str);
//!Parses the given string, returns the dnot root node.
dnot_token	dnot_parse_string(const std::string& str);
}

/*
Dnot puede leer algo como...

inicio:1,
sencillo:{
	ps1:"una cadena con espacios y : y cosas así, con commas y {abre} y [cierra]",
	ps2:12
34
},

complejo:{
	parametro_complejo_uno:true,
	parametro_complejo_dos:{
		cosa_uno: "Hola dos puntos : hola",
		cosa_dos:2.34f,
		cosa_tres:2.340343243
	},
	parametro_complejo_tres:-3,
	parametro_complejo_cuatro:{
		p_a:{
			uno:1,
			dos:2},
		p_b:"beee"
	},
	tarray:[1,2,3, {uno:1, dos:2}, 4, 5, ["uno", "dos", "tres"]],
	obj:{a:"b", c:"d"}
}

Ignorará todo whitespace que no esté en una cadena y tolerará valores enteros,
de cadena, float a bool.

Un ejemplo de uso está en "base_proyecto", en github.
*/
#endif
