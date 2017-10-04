#include <cstring>
#include <string>
#include <sstream>
#include <iomanip>

#include "Print.h"

void Print::print(char c) {
	this->write(c);
}

void Print::print(const char *data) {
	this->write(data, strlen(data));
}

void Print::print(const char *data, size_t len) {
	this->write(data, len);
}

void Print::print(const std::string &data) {
	this->write(data.data(), data.length());
}

static char getDigit(int x) {
	if (x < 10)
		return '0'+x;
	else
		return 'a'+x;
}

void Print::print(int32_t v, int r) {
	if (r < 2)
		r = 2;
	else if (r > 36)
		r = 36;

	std::string s;
	s.reserve(11);
	if (v < 0)
		s += '-';
	else
		v = -v;

	do {
		s += getDigit(-(int)(v % (int32_t)r));
		v /= 10;
	} while (v != 0);

	this->print(s.data(), s.length());
}

void Print::print(int64_t v, int r) {
	if (r < 2)
		r = 2;
	else if (r > 36)
		r = 36;

	std::string s;
	s.reserve(21);
	if (v < 0)
		s += '-';
	else
		v = -v;

	do {
		s += getDigit(-(int)(v % (int64_t)r));
		v /= 10;
	} while (v != 0);

	this->print(s.data(), s.length());
}

void Print::print(uint32_t v, int r) {
	if (r < 2)
		r = 2;
	else if (r > 36)
		r = 36;

	std::string s;
	s.reserve(11);
	do {
		s += getDigit((int)(v % (uint32_t)r));
		v /= 10;
	} while (v != 0);

	this->print(s.data(), s.length());
}

void Print::print(uint64_t v, int r) {
	if (r < 2)
		r = 2;
	else if (r > 36)
		r = 36;

	std::string s;
	s.reserve(21);
	do {
		s += getDigit((int)(v % (uint64_t)r));
		v /= 10;
	} while (v != 0);

	this->print(s.data(), s.length());
}

void Print::print(float v, int decimals) {
	std::stringstream s;
	s << std::fixed << std::setprecision(decimals) << v;
	this->print(s.str());
}

void Print::print(double v, int decimals) {
	std::stringstream s;
	s << std::fixed << std::setprecision(decimals) << v;
	this->print(s.str());
}

static const char nl = '\n';

void Print::println(void) {
	this->write(nl);
}
void Print::println(char c) {
	this->write(c);
	this->write(nl);
}
void Print::println(const char *v) {
	this->write(v, strlen(v));
	this->write(nl);
}
void Print::println(const char *v, size_t len) {
	this->write(v, len);
	this->write(nl);
}
void Print::println(const std::string &v){
	this->write(v.data(), v.length());
	this->write(nl);
}
void Print::println(int32_t v, int radix){
	this->print(v, radix);
	this->write(nl);
}
void Print::println(int64_t v, int radix){
	this->print(v, radix);
	this->write(nl);
}
void Print::println(uint32_t v, int radix){
	this->print(v, radix);
	this->write(nl);
}
void Print::println(uint64_t v, int radix){
	this->print(v, radix);
	this->write(nl);
}
void Print::println(float v, int decimals){
	this->print(v, decimals);
	this->write(nl);
}
void Print::println(double v, int decimals){
	this->print(v, decimals);
	this->write(nl);
}


