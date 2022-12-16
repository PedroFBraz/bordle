#pragma once
#include <string>
#include <cstdlib>
#include "spdlog/spdlog.h"

void assert_(bool assertion, std::string error_message) {
	if (!assertion) {
		spdlog::error(error_message);
		std::exit(1);
	}
}
