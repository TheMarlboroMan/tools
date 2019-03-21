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
					i8n_exception_file_error(const std::string&);
};

//!Thrown when trying to add a file with an already existing key.
class i8n_exception_repeated_key
	:public i8n_exception {
					i8n_exception_repeated_key(const std::string&);
}

class i8n {

	public:

	typedef t_filekey		std::string; //!<Key for a file.
	typedef t_entrykey		std::string; //<!Key for an entry.

	struct substitution {
		std::string		key,
					value;
	};

	//!Class constructor.
				i8n();

	//!Class constructor with path and default language.
				i8n(const std::string&, const std::string&);

	//!Adds a key to the database, associated to the given file.
	//!Will throw on failure to open the file,  if the key is used
	//!or if no path/language have been set.
	void			add(const t_filekey&, const std::string&);

	//!Adds a permanent substitution.
	void			set(const std::string&, const std::string&);

	//!Sets the root of the files in the filesystem. Will reload the database of texts.
	void			set_root(const std::string&);

	//!S.ets the current language key. Will reload the database of texts.
	void			set_language(const std::string&);
	
	//!Retrieves - from the key database - the given text.
	//!Returns a fail string if not found.
	std::string		get(const t_filekey&, const t_entrykey&) const;

	//!Retrieves - from the key database - the given text performing the
	//!substitions passed. Returns a fail string if not found.
	std::string		get(const t_filekey&, const t_entrykey&, const std::vector<substitution>&) const;

	private:

	class parser {

		public:

		//!Class constructor.
					parser(codex_entry&, std::ofstream&);
		//!Parses the file into the entry.
		void			parse();

		private:

		const std::string	open_entry="[:",
					close_entry="]:",
					open_var="(:",
					close_var":)",
					open_embed="<:",
					close_embed=":>";

		const char 		comment='#';

		void			parse_scan(const char, std::string&);
		void			parse_key(const char);
		void			parse_value(const char);
		void			control(const char);

		codex_entry&		entry;
		std::ofstream&		file;
		std::array<char, 2>	control_buffer;
		std::string		buffer;

		enum class modes	{scan, key, value} mode=scan;
	};

	struct codex_entry {

							codex_entry(const std::string);
		std::string 				path;		//<!Name of a file.
		std::map<t_entrykey, std::string> 	entries;	//<!Entries.
		//!Removes entries.
		void					clear();
	};

	//!Reloads all entries.
	void			reload_codex();

	std::string			file_path,	//<!File path where files are located.
					language;	//<!Language string, must be a subdirectory of the file_path.

	std::vector<substitution>	substitutions;	//<!Permanent substitutions.
	std::map<t_filekey, codex_entry>		codex;	//<!All data.
};

}

#endif
