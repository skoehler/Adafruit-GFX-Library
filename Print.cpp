#include <cstring>

#include "Print.h"

size_t Print::print(char c) {
	return this->write(c);
}

size_t Print::print(const char *data) {
	return this->write(data, strlen(data));
}

size_t Print::print(const char *data, size_t len) {
	return this->write(data, len);
}

size_t Print::print(const std::string &data) {
	return this->print(data.data(), data.length());
}
