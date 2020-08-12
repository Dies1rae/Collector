#pragma once
#include "logg.h"
#include <Windows.h>
#include <winevt.h>

class collector{
private:
	std::vector<std::string> files_to_copy_in_root_;
	logg * main_log_container_;
	std::string root_folder_;
public:
	collector():main_log_container_(NULL), root_folder_(".\\"){};
	collector(logg* L):main_log_container_(L), root_folder_(".\\") {};
	~collector() {};
	void init(logg* L);
	void run();
	std::string get_pc_name();
	std::string get_pc_CPU_info();
	void get_pc_disk_space();
	void get_pc_motherboard_info();
	std::string get_pc_ram_info();
	void create_root_folder();
	void collect_log_file();
	void move_collected_to_root();
};

