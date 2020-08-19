#pragma once
#define WIN32_LEAN_AND_MEAN
#include "logg.h"
#include <Windows.h>
#include <winevt.h>

class collector{
private:
	bool key_;
	logg * main_log_container_;
	std::string root_folder_;
	std::vector<std::string> files_to_copy_in_root_;
public:
	//crate, init and main loop
	collector() :main_log_container_(NULL), root_folder_(".\\"), key_(false) {};
	collector(logg* L) :main_log_container_(L), root_folder_(".\\"), key_(false) {};
	~collector() {};
	void init(logg* L, bool K);
	void run();
private:
	//all serv methods
	std::string get_pc_name();
	std::string get_pc_CPU_info();
	void get_pc_disk_space();
	std::string get_pc_motherboard_info();
	std::string get_pc_ram_info();
	std::string get_pc_videodev_info();
	std::vector<std::string> get_pc_network_hard_info();
	std::string get_pc_network_soft_info();
	void create_root_folder_and_move_logs();
	void move_collected_to_root();
	void collect_log_file();
	std::vector<std::string> get_installed_software();
};