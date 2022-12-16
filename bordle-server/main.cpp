#define BORDLE_DEBUG
#include <string>
#include <iostream>
#include <format>
#include <algorithm>
#include <chrono>
#include <fstream>
#include "util.h"
#include "toml.hpp"
#include "sqlite3.h"

#if defined(_WIN32)
	#define BORDLE_VFS "win32"
#elif defined(__unix__)
	#define BORDLE_VFS "unix"
#else
	#error Unsupported operating system
#endif



void print_error(std::string error_message) {
	std::cout << error_message << std::endl;
}


int main() {
	
	const auto data = toml::parse("config.toml");
#pragma region Asserts
	assert_(toml::find<long long>(data, "port") <= 65'535,
			"Assertion failed! port variable in config.toml must not be greater than 65'535!",
			print_error);
	assert_(toml::find<long long>(data, "port") > 0,
		    "Assertion failed! port variable in config.toml must be greater than 0!",
		    print_error);
	assert_(toml::find<long long>(data, "max_attempts") <= 65'535,
		    "Assertion failed! attempt_count variable in config.toml must not be greater than 65'535!",
		    print_error);
	assert_(toml::find<long long>(data, "max_attempts") > 0,
		    "Assertion failed! attempt_count variable in config.toml must be greater than 0!",
		    print_error);
	assert_(toml::find<std::string>(data, "db_name").length() <= 1024,
			"Assertion failed! db_name variable length must not be greater than 1024!",
			print_error);
	assert_(!toml::find<std::string>(data, "db_name").empty(),
			"Assertion failed! db_name variable must not be an empty string!",
			print_error);
	assert_(toml::find<long long>(data, "word_size") <= 65'535,
			"Assertion failed! word_size variable must not be greater than 65'535!",
			print_error);
	assert_(toml::find<long long>(data, "word_size") > 0,
			"Assertion failed! word_size variable must be greater than 0!",
			print_error);
#pragma endregion
	const auto port			= toml::find<unsigned short>(data, "port");
	const auto max_attempts = toml::find<unsigned short>(data, "max_attempts");
	//const auto ip_address	= toml::find<std::string>(data, "ip_address"); // What's this here for lol?
	const auto db_name		= toml::find<std::string>(data, "db_name");
	const auto create_db    = toml::find<bool>(data, "create_db");
	const auto word_size	= toml::find<unsigned short>(data, "word_size");
	sqlite3* db = nullptr;
	sqlite3_open_v2(db_name.c_str(), &db, (create_db ? SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE : SQLITE_OPEN_READWRITE), BORDLE_VFS);
	assert_(sqlite3_errcode(db) != SQLITE_CANTOPEN, "Assertion failed! Cannot open database file."
		   "\nHint: You might never have created a database file."
		   "\nHint: You might have set the create_db variable in config.toml to false.",
		  print_error);
	// Todo: Ensure that name isn't bigger than 255
	std::string sql_statement = std::format("CREATE TABLE IF NOT EXISTS users (ip CHARACTER(32),"
											"name CHARACTER(255), correct INT, wrong INT,"
											"got_today_correct INT);"
											"CREATE TABLE IF NOT EXISTS bordles (word VARCHAR({}),"
											"date DATE,"
											"UNIQUE(word));", word_size);
	char*  sqlite_error_message = static_cast<char*>(sqlite3_malloc(1024));
	std::fill(sqlite_error_message, sqlite_error_message + 1023, '\0');
	sqlite3_exec(db, sql_statement.c_str(), nullptr, nullptr, &sqlite_error_message);
	//assert_(!sqlite_error_message, sqlite_error_message, print_error); // Buggy
	const auto current_time = std::chrono::system_clock::now().time_since_epoch().count();
	constexpr auto day_in_seconds = 86'400;
	std::ifstream words("words.txt");
	assert_(words.is_open(), "Assertion failed! Failed to open words.txt", print_error); 
	for (unsigned long long i = current_time; !words.eof(); i += day_in_seconds) {
		std::string word;
		words >> word;
		//assert_(word.size() == word_size, std::vformat("Assertion failed! {:x} is {} than {}!", std::make_format_args(word, (word.size() > word_size ? "bigger" : "smaller"), word_size)), print_error);
		std::string sql_statement = std::format("INSERT OR IGNORE INTO bordles VALUES ('{}', {});", word, i); // This probably isn't efficient
		
		//	If there is a last empty newline in words.txt, it gets inserted into the database.
		if (!word.empty()) {
			sqlite3_exec(db, sql_statement.c_str(), nullptr, nullptr, &sqlite_error_message);
		}
		//assert_(!sqlite_error_message, sqlite_error_message, print_error); // Buggy
	}
	words.close();
	sqlite3_free(sqlite_error_message);
	sqlite_error_message = nullptr;
}