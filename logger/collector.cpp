#include "collector.h"
#include "places.h"
#include <filesystem>
#include <string>
#include <iostream>
#include <WinBase.h>
#include <Windows.h>
#include <fstream>
#include <sstream>
namespace fs = std::filesystem;

void collector::init(logg* L) {
	this->main_log_container_ = L;
	char ch[14] = "INIT DONE";
	this->main_log_container_->add_log_string(ch);
}

void collector::run() {
	char che[18] = "LOGGING END";
	char chb[20] = "LOGGING BEGIN";
	this->main_log_container_->add_log_string_timemark_(chb);
	this->main_log_container_->write_to_file();
	this->main_log_container_->add_log_string("---MAIN INFO---");
	std::string pc_name = get_pc_name();
	this->root_folder_ += pc_name+'\\';
	this->main_log_container_->add_log_string(pc_name);
	this->main_log_container_->add_log_string(get_pc_sys_info());


	this->main_log_container_->add_log_string("");
	this->main_log_container_->add_log_string_timemark_(che);
	this->main_log_container_->write_to_file();
	this->files_to_move_in_root_.push_back(this->main_log_container_->get_log_path());
	this->create_root_folder();
	this->move_collected_to_root();
}

std::string collector::get_pc_name() {
	std::cout << "--PC NAME--" << std::endl;
	TCHAR computerName[MAX_COMPUTERNAME_LENGTH + 1];
	DWORD size = sizeof(computerName) / sizeof(computerName[0]);
	GetComputerName(computerName, &size);
	std::wstring tmp = computerName;
	std::string CompN(tmp.begin(), tmp.end());
	std::cout << CompN << std::endl;
	return CompN;
}

std::string collector::get_pc_sys_info() {
	std::cout << "--PROC INFO--" << std::endl;
	std::string cpu_info;
	SYSTEM_INFO sinfo;
	GetSystemInfo(&sinfo);
	if (sinfo.wProcessorArchitecture == 0) {
		std::cout << "ARCH NUM: " <<sinfo.wProcessorArchitecture << " _INTEL_X86" << "|CORES: " << sinfo.dwNumberOfProcessors << std::endl;
		cpu_info += "ARCH NUM: " + std::to_string(sinfo.wProcessorArchitecture) + " _INTEL_X86" + "|CORES: " + std::to_string(sinfo.dwNumberOfProcessors);
	}
	if (sinfo.wProcessorArchitecture == 9) {
		std::cout << "ARCH NUM: " << sinfo.wProcessorArchitecture << " x64_(AMD or Intel)" << "|CORES: " << sinfo.dwNumberOfProcessors << std::endl;
		cpu_info += "ARCH NUM: " + std::to_string(sinfo.wProcessorArchitecture) + " x64_(AMD or Intel)" + "|CORES: " + std::to_string(sinfo.dwNumberOfProcessors);
	}
	if (sinfo.wProcessorArchitecture == 5) {
		std::cout << "ARCH NUM: " << sinfo.wProcessorArchitecture << " ARM" << "|CORES: " << sinfo.dwNumberOfProcessors << std::endl;
		cpu_info += "ARCH NUM: " + std::to_string(sinfo.wProcessorArchitecture) + " ARM" + "|CORES: " + std::to_string(sinfo.dwNumberOfProcessors);
	}
	if (sinfo.wProcessorArchitecture == 12) {
		std::cout << "ARCH NUM: " << sinfo.wProcessorArchitecture << " ARM64" << "|CORES: " << sinfo.dwNumberOfProcessors << std::endl;
		cpu_info += "ARCH NUM: " + std::to_string(sinfo.wProcessorArchitecture) + " ARM64" + "|CORES: " + std::to_string(sinfo.dwNumberOfProcessors);
	}
	if (sinfo.wProcessorArchitecture == 6) {
		std::cout << "ARCH NUM: " << sinfo.wProcessorArchitecture << " Intel_Itanium-based" << "|CORES: " << sinfo.dwNumberOfProcessors << std::endl;
		cpu_info += "ARCH NUM: " + std::to_string(sinfo.wProcessorArchitecture) + " Intel_Itanium-based" + "|CORES: " + std::to_string(sinfo.dwNumberOfProcessors);
	}
	if(sinfo.wProcessorArchitecture == 0xffff) {
		std::cout << "ARCH NUM: " << sinfo.wProcessorArchitecture << " Unknown_arch" << "|CORES: " << sinfo.dwNumberOfProcessors << std::endl;
		cpu_info += "ARCH NUM: " + std::to_string(sinfo.wProcessorArchitecture) + " Unknown_arch" + "|CORES: " + std::to_string(sinfo.dwNumberOfProcessors);
	}
	return cpu_info;
}

void collector::create_root_folder() {
	std::cout << "--CREATE DIR--" << std::endl;
	if (!fs::is_directory(this->root_folder_) || !fs::exists(this->root_folder_)) {
		fs::create_directory(this->root_folder_);
		std::cout << "--CREATED::" << this->root_folder_ << std::endl;
		return;
	}
	std::cout << "--CREATED::" << this->root_folder_ << std::endl;
}

void collector::move_collected_to_root() {
	for (auto& ptr : this->files_to_move_in_root_) {
		std::ifstream src_file(ptr, std::ios::binary);
		std::string only_file_name = ptr.substr(ptr.find_last_of('\\'));
		std::string dest_file_path = this->root_folder_ + only_file_name;
		std::ofstream dest_file(dest_file_path, std::ios::binary);
		dest_file << src_file.rdbuf();
		src_file.close();
		dest_file.close();
	}
}