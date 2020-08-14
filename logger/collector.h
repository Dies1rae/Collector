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
	//crate, init and main loop
	collector():main_log_container_(NULL), root_folder_(".\\"){};
	collector(logg* L):main_log_container_(L), root_folder_(".\\") {};
	~collector() {};
	void init(logg* L);
	void run();
protected:
	//all serv methods
	std::string get_pc_name();
	std::string get_pc_CPU_info();
	void get_pc_disk_space();
	std::string get_pc_motherboard_info();
	std::string get_pc_ram_info();
	std::string get_pc_videodev_info();
	std::vector<std::string> get_pc_network_info();
	void create_root_folder_and_move_logs();
	void collect_log_file();
	void move_collected_to_root();
};

