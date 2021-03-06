#define _CRT_SECURE_NO_WARNINGS
#define WIN32_LEAN_AND_MEAN
#pragma once
#include <iostream>
#include <ctime>
#include <chrono>
#include <iomanip>
#include <string>
#include <fstream>
#include <vector>

class logg {
private:
	std::string logpath;
	std::vector<std::string> logd;
public:
	logg() :logpath(".\\log.txt"), logd() {};
	logg(const std::string& mess){
		this->logpath = ".\\log.txt";
		this->logd.push_back(mess);
	};
	~logg() {}
	void add_log_string(std::string L);
	void add_log_string(char* L);
	void add_log_string_timemark_(std::string L);
	void add_log_string_timemark_(char* L);
	void write_to_file();
	std::string get_log_path();
	void set_logpath(std::string lp);
};