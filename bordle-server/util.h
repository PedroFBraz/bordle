#pragma once
#include <string>
#include <cstdlib>

template <typename Error_handler>
void assert_(bool assertion, std::string error_message, Error_handler error_handler) {
	if (!assertion) {
		error_handler(error_message);
		std::exit(1);
	}
}
