#pragma once
#include "logg.h"
#include <Windows.h>
#include <winevt.h>

class collector{
private:
	std::vector<std::string> basic_info_container_;
	std::vector<std::string> files_to_move_in_root_;
	logg * main_log_container_;
	std::string root_folder_;
public:
	collector():basic_info_container_(), main_log_container_(NULL), root_folder_(".\\"){};
	collector(logg* L):basic_info_container_(), main_log_container_(L), root_folder_(".\\") {};
	~collector() {};
	void init(logg* L);
	void run();
	std::string get_pc_name();
	std::string get_pc_sys_info();
	void create_root_folder();
	void collect_file_to_move();
	void move_collected_to_root();
};

