#ifndef TOOLS_I8N
#define TOOLS_I8N

#include <string>
#include <vector>
#include <array>
#include <map>
#include <stdexcept>
#include <fstream>

namespace tools {

//!Base exception for the module.
class i8n_exception
	:public std::runtime_error {
	public:
					i8n_exception(const std::string&);
};

//!Thrown when calling "add" when no path has been specified.
class i8n_exception_no_path
	:public i8n_exception {
	public:
					i8n_exception_no_path();
};

//!Thrown when calling "add" when no language has been specified.
class i8n_exception_no_language
	:public i8n_exception {
	public:
					i8n_exception_no_language();
};

//!Thrown when trying to retrieve data from an empty i8n class.
class i8n_exception_no_data
	:public i8n_exception {
	public:
					i8n_exception_no_data();
};

//!Thrown when a file cannot be added.
class i8n_exception_file_error
	:public i8n_exception {
	public:
					i8n_exception_file_error(const std::string&);
};

//!Thrown when trying to add a file with an already existing key.
class i8n_exception_repeated_key
	:public i8n_exception {
	public:
					i8n_exception_repeated_key(const std::string&);
};

//!Visible lexer exception, with path, line, error and line number.
class i8n_lexer_error
	:public i8n_exception {
	public:
					i8n_lexer_error(const std::string&, const std::string&, const std::string&, int);
};

//!Minimal lexer exception, to be thrown from within
class i8n_lexer_generic_error
	:public std::runtime_error {
	public:
					i8n_lexer_generic_error(const std::string&);
};

//!Unexpected tokens when parsing, inner use.
class i8n_parser_token_error
	:public i8n_exception {
	public:
					i8n_parser_token_error(const std::string, int, int);
};

//!Failure to parse, visible from the outside.
class i8n_parser_error
	:public i8n_exception {
	public:
					i8n_parser_error(const std::string);
};

class i8n {

	public:

	//!Fed to "get" methods, to substitute variables.
	struct substitution {
		std::string		key,
						value;
	};

	//!Fet to "get" methods to indicate key and entry.
	struct i8n_get {
		const std::string	key,
					entry;
	};

	//!Class constructor with path, default language and list of files.
							i8n(const std::string&, const std::string&, const std::vector<std::string>&);

	//!Adds a key to the database, associated to the given file.
	//!Will throw on failure to open the file,  if the key is used
	//!or if no path/language have been set. This will trigger a recompilation
	//!of all texts.
	void					add(const std::string&);

	//!Adds a permanent substitution.
	void					set(const substitution&);

	//!Sets the root of the files in the filesystem. Will reload the database of texts.
	void					set_root(const std::string&);

	//!Sets the current language key. Will reload the database of texts
	//!and trigger a recompilation.
	void					set_language(const std::string&);

	//!Retrieves - from the key database - the given text.
	//!Returns a fail string if not found.
	std::string				get(const i8n_get&) const;

	//!Retrieves - from the key database - the given text performing the
	//!substitions passed. Returns a fail string if not found.
	std::string				get(const i8n_get&, const std::vector<substitution>&) const;

	private:

	//!Reloads all entries.
	void					reload_codex();
	//!Internally adds a file, does not trigger recompilation.
	void					add_private(const std::string&);
	//!Compiles the lexer tokens into the codex entries.
	void					build_entries();

	std::string				file_path,	//<!File path where files are located.
							language;	//<!Language string, must be a subdirectory of the file_path.

	std::map<std::string, std::string>		substitutions;	//<!Permanent substitutions.

	//!Files are resolved to entries (one entry per data item). Each entry is
	//!composed by segments, which represent a fixed test or a variable to be resolved.
	struct entry_segment {
		enum class types {literal, variable, embed}	type;	//!<These are the different entry types. Embed should only exist when compiling.
		std::string									value;
	};

	//!Entry in the i8n dictionary.
	struct codex_entry {
		std::vector<entry_segment>		segments;
	};

	std::map<std::string, codex_entry>		codex;	//<!All data.

	//!Internal lexer: converts files into streams of tokens.
	class lexer {
		public:

		//!The different ytpes
		enum class tokentypes {openlabel, closelabel, openvalue, closevalue,
			openvar, closevar, openembed, closeembed, nothing, literal};

		//!Represents a single lexer token (linguistic token or literal).
		struct token {
			tokentypes	type;
			std::string	val;
			int			line, charnum;
		};

		static std::string	typetostring(tokentypes);

		//!Processes the file of the given filename. Returns a list of
		//!lexer tokens.
		std::vector<token>	process(const std::string&);

		private:

		//!Scans two characters to see if they correspond with delimiters,
		//!returning the token type (nothing if none detected).
		tokentypes		scan_buffer(const std::string&);

		//Delimiters...
		const std::string	open_label="[/",
							close_label="/]",
							open_value="{/",
							close_value="/}",
							open_var="(/",
							close_var="/)",
							open_embed="</",
							close_embed="/>";

		const char 			comment='#';
	};

	std::map<std::string, std::vector<lexer::token>>		lexer_tokens;	//<!Temporary map of file to a vector of tokens.

	//!Internal parser, converts tokens into codex entries. Given that codex
	//!entries can have dependencies between them, this also solves then in
	//!as many passes as needed. Circular dependencies cause it to throw.
	class parser {

		public:
		//!Class constructor. Created from a map of file names to lexer tokens.
						parser(const std::map<std::string, std::vector<lexer::token>>&);

		private:

		//!Prints out the tokens to the given stream, for debug purposes.
		void			debug_tokens(const std::vector<lexer::token>&, std::ostream&);
		void			debug_token(const lexer::token&, std::ostream&);
		void			debug_entry(const codex_entry&, std::ostream&);
		void			debug_segment(const entry_segment&, std::ostream&);
		//!Parsers all the tokens from a file.
		void			parse_file(const std::vector<lexer::token>&);
		//!Starts the label phase: skip all whitespace until a label is reached, store the label. Returns false if there is no error and the tokens ended.
		bool 			label_phase(const std::vector<lexer::token>& _tokens, int& _curtoken, const int _size);
		//!Parse the value contents until a "close value" is found.
		void 			value_phase(const std::vector<lexer::token>& _tokens, int& _curtoken, const int _size);
		//!Returns the index of the next token that maches type, skipping whitespace literals, from _curtoken. Any other type than whitespace literals will throw!.
		int				find_next_of(const std::vector<lexer::token>& _tokens, lexer::tokentypes _type, int _curtoken);
		//!Parses the inner component open+identifier+close from _curtoken, call the callback with the middle token after checking it is a literal.
		void			parse_open_close(const std::vector<lexer::token>& _tokens, lexer::tokentypes _closetype, int _curtoken, void(parser::*)(const lexer::token&));
		//!Callback that will add a new entry to "entries" and set current_label.
		void			create_entry(const lexer::token&);
		//!Callback to add a new embed element to the current codex_entry.
		void			add_embed(const lexer::token&);
		//!Callback to add a new variable element to the current codex_entry.
		void			add_var(const lexer::token&);
		//!In compile mode, returns true if a codex entry has no embed dependencies left.
		bool 			is_entry_solved(const codex_entry& _entry, const std::map<std::string, codex_entry>& _solved) const;
		//!In compile mode, tries to replace all embed entries with their resulting static or variable segments.
		bool 			solve_entry(codex_entry& _entry, const std::map<std::string, codex_entry>& _solved);

		std::map<std::string, codex_entry>		entries;		//<!Entries that have been added, yet unsolved.
		std::string								current_label;	//<!Label that is currently in process.
	};
};

}

#endif
