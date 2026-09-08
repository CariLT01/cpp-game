#include "io.hpp"
#include <fstream>
#include <stdexcept>
#include <sstream>

std::string readFile(const std::string& filename) {
	std::ifstream file(filename);
	if (!file.is_open()) {
		throw std::runtime_error("Failed to open file: " + filename);
	}

	std::stringstream sstream;
	sstream << file.rdbuf();

	return sstream.str();
}