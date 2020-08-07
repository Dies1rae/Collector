#pragma once
#include "logg.h"
#include <Windows.h>
#include <winevt.h>

class collector{
private:
	std::vector<std::string> basic_info_container_;
	logg * main_log_container_;
public:
	collector():basic_info_container_(), main_log_container_(NULL){};
	collector(logg* L):basic_info_container_(), main_log_container_(L) {};
	~collector() {};
	void init(logg* L);
	void run();
	std::string get_pc_name();
	std::string get_pc_sys_info();
};

