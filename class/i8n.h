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
}

//!Thrown when a syntax error is found parsing a file.
class i8n_exception_parse_syntax_error
	:public i8n_exception {
	public:
					i8n_exception_parse_syntax_error(const std::string&, const std::string&, const std::string&, int);
	}

	const std::string		error,
					file,
					line;
	const int			linenum;
};

//!Generic, contextless parse error.
class i8n_exception_parse_error
	:public i8n_exception {
	public:
					i8n_exception_parse_error(const std::string&);
};

//!Base exception for the lexer to be thrown from within
class i8n_lexer_generic_exception {
	:public i8n_exception {
		public:			i8n_lexer_generic_exception(const std::string&);
};

class i8n {

	public:

	//!Fed to the class, indicates a file and a key with which to identify it.
	struct 				i8n_input{
		const std::string	key,
					path;
	};

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
				i8n(const std::string&, const std::string&, const std::vector<i8n_input>&);

	//!Adds a key to the database, associated to the given file.
	//!Will throw on failure to open the file,  if the key is used
	//!or if no path/language have been set. This will trigger a recompilation
	//!of all texts.
	void			add(const i8n_input&);

	//!Adds a permanent substitution.
	void			set(const std::string&, const std::string&);

	//!Sets the root of the files in the filesystem. Will reload the database of texts.
	void			set_root(const std::string&);

	//!Sets the current language key. Will reload the database of texts
	//!and trigger a recompilation.
	void			set_language(const std::string&);
	
	//!Retrieves - from the key database - the given text.
	//!Returns a fail string if not found.
	std::string		get(const i8n_get&) const;

	//!Retrieves - from the key database - the given text performing the
	//!substitions passed. Returns a fail string if not found.
	std::string		get(const i8n_get&, const std::vector<substitution>&) const;

	private:

	//!Reloads all entries.
	void					reload_codex();
	//!Internally adds a file, does not trigger recompilation.
	void					add_private(const i8n_input&);
	//!Builds the codex entries, resolving their relationships.
	void					build_entries();

	std::string				file_path,	//<!File path where files are located.
						language;	//<!Language string, must be a subdirectory of the file_path.

	std::vector<substitution>		substitutions;	//<!Permanent substitutions.
	std::map<std::string, codex_page>	codex;	//<!All data.

	//!Internal lexer.
	class file_lexer {
		public:

		struct token {
			tokentypes	type;
			std::string	val;
		};

		//!Processes the file of the given filename.
		std::vector<token>	process(const std::string&);

		private:

		enum class tokentypes {openlabel, closelabel, openvalue, closevalue,
			openvar, closevar, openembed, closeembed, nothing, literal};

		tokentypes		scan_buffer(const std::string&);


		//Delimiters...
		const std::string	open_label="[/",
					close_label="/]",
					open_value="{/",
					close_label="/}",
					open_var="(/",
					close_var"/)", //<-- TODO: This looks like a smiley... DANGER
					open_embed="</",
					close_embed="/>";

		const char 		comment='#';
	};

	//!Internal file parser.
	class file_parser {

		public:

		//!Class constructor.
					file_parser(codex_page&, std::ofstream&);
		//!Parses the file into the entry.
		void			parse();

		private:

		//TODO: These will be useless once the lexer enters.
		//Delimiters...
		const std::string	open_entry="[:",
					close_entry="]:",
					open_var="(:",
					close_var":)",
					open_embed="<:",
					close_embed=":>";

		const char 		comment='#';

		void			parse_scan(const char, std::string&);
		void			parse_key(const char, const std::string&);
		void			parse_value(const char, std::string&);
		void			control(const char);

		//TODO: Not really: the parser should keep a list of keys to values
		//and then compile them into a page.
		codex_page&		page;
		std::ofstream&		file;
		std::string		buffer,
					control_buffer,
					key;

		enum class modes	{scan, key, value} mode=scan;
	};

	//!Each file is compiled into a codex_page, which maps a key to a 
	//!an entry, itself a sequence of static texts and variables (called
	//!a segment).
	struct codex_page {

							codex_page(const std::string);
		std::string 				path;		//<!Name of a file.
		std::map<std::string, codex_entry> 	entries;	//<!Entries.
		//!Removes entries.
		void					clear();
	};

	//Sequence of variables and static strings.
	struct codex_entry {
		std::vector<entry_segment>		segments;
	}

	//!Represents a fixed test or a variable to be resolved.
	struct entry_segment {
		enum class type {static, variable}	type;
		std::string				value;
	}
};

}

#endif
