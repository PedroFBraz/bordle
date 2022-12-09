#include <string>
#include <iostream>
#include <format>
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

	assert_(toml::find<long long>(data, "port") <= 65'535,
			"Assertion failed! port variable in config.toml is too big! Maximum is 65'535",
			print_error);
	assert_(toml::find<long long>(data, "port") > 0,
		    "Assertion failed! port variable in config.toml must be greater than 0!",
		    print_error);
	assert_(toml::find<long long>(data, "max_attempts") <= 65'535,
		    "Assertion failed! attempt_count variable in config.toml is too big! Maximum is 65'535",
		    print_error);
	assert_(toml::find<long long>(data, "max_attempts") > 0,
		    "Assertion failed! attempt_count variable in config.toml must be greater than 0!",
		    print_error);

	const auto port			= toml::find<unsigned short>(data, "port");
	const auto max_attempts = toml::find<unsigned short>(data, "max_attempts");
	const auto ip_address	= toml::find<std::string>(data, "ip_address");
	const auto db_name		= toml::find<std::string>(data, "db_name"); // Todo: Add checks to see if the string is empty and check for size (or else sqlite returns an error if the file name is too long)
	const auto create_db    = toml::find<bool>(data, "create_db");
	const auto word_size	= toml::find<unsigned short>(data, "word_size"); // Todo: Add checks
	sqlite3* db = nullptr;
	sqlite3_open_v2(db_name.c_str(), &db, (create_db ? SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE : SQLITE_OPEN_READWRITE), BORDLE_VFS);
	assert_(sqlite3_errcode(db) != SQLITE_CANTOPEN, "Assertion failed! Cannot open database file."
		   "\nHint: You might never have created a database file."
		   "\nYou might have set the create_db variable in config.toml to false",
		  print_error);

	// Todo: Ensure that name isn't bigger than 255
	std::string sql_statement = std::format("CREATE TABLE IF NOT EXISTS users (ip CHARACTER(32),"
											"name CHARACTER(255), correct INT, wrong INT,"
											"got_today_correct INT);"
											"CREATE TABLE IF NOT EXISTS bordles (word VARCHAR({}),"
											"date DATE);", word_size);
	sqlite3_exec(db, sql_statement.c_str(), nullptr, nullptr, nullptr); // Todo: Perform error handling
}