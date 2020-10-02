#include "logg.h"

void logg::write_to_file() {
	std::ofstream log_file(this->logpath, std::ios::app);
	for (std::string& ptr : this->logd) {
		log_file << ptr << '\n';
	}
	log_file.close();
	this->logd.clear();
}


void logg::add_log_string_timemark_(std::string L) {
	time_t now = std::time(NULL);
	time(&now);
	L.insert(0, "*");
	L += "::";
	L += ctime(&now);
	this->logd.push_back(L);
}

void logg::add_log_string_timemark_(char* L) {
	std::string out(L);
	time_t now = std::time(NULL);
	time(&now);
	out.insert(0, "*");
	out += "::";
	out += ctime(&now);
	this->logd.push_back(out);
}

void logg::add_log_string(std::string L) {
	if (L.size() > 0) {
		this->logd.push_back(L);
	}
	else {
		this->logd.push_back(L);
	}
}

void logg::add_log_string(char * L) {
	std::string out(L);
	this->logd.push_back(out);
}

std::string logg::get_log_path() {
	return this->logpath;
}
void logg::set_logpath(std::string lp) {
	this->logpath.clear();
	this->logpath = lp;
}
