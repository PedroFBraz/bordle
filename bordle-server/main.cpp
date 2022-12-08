#include <string>
#include <iostream>
#include "util.h"
#include "toml.hpp"

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
	assert_(toml::find<long long>(data, "max_attempts" > 0),
		    "Assertion failed! attempt_count variable in config.toml must be greater than 0!",
		    print_error);

	const auto port			= toml::find<unsigned short>(data, "port");
	const auto max_attempts = toml::find<unsigned short>(data, "max_attempts");
	const auto ip_address	= toml::find<std::string>(data, "ip_address");
}