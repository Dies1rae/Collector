#include "collector.h"
#include "places.h"
#include <filesystem>
#include <string>
#include <iostream>
#include <WinBase.h>
#include <Windows.h>
#include <fstream>
#include <sstream>
#include <intrin.h>
#include <vector>

namespace fs = std::filesystem;

void collector::init(logg* L) {
	this->main_log_container_ = L;
	char ch[14] = "INIT DONE";
	this->main_log_container_->add_log_string(ch);
	this->files_to_copy_in_root_.push_back(this->main_log_container_->get_log_path());
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
	this->main_log_container_->add_log_string(get_pc_CPU_info());
	this->main_log_container_->add_log_string(get_pc_ram_info());
	get_pc_disk_space();


	this->collect_log_file();
	this->main_log_container_->add_log_string("");
	this->main_log_container_->add_log_string_timemark_(che);
	this->main_log_container_->write_to_file();
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

std::string collector::get_pc_CPU_info() {
	int CPUInfo[4] = { -1 };
	__cpuid(CPUInfo, 0x80000000);
	unsigned int nExIds = CPUInfo[0];
	// Get the information associated with each extended ID.
	char CPUBrandString[0x40] = { 0 };
	
	for (unsigned int ptr = 0x80000000; ptr <= nExIds; ++ptr)	{
		__cpuid(CPUInfo, ptr);

		// Interpret CPU brand string and cache information.
		if (ptr == 0x80000002){
			memcpy(CPUBrandString, CPUInfo, sizeof(CPUInfo));
		}
		if (ptr == 0x80000003){
			memcpy(CPUBrandString + 16,CPUInfo,sizeof(CPUInfo));
			
		}
		if (ptr == 0x80000004){
			memcpy(CPUBrandString + 32, CPUInfo, sizeof(CPUInfo));
		}
	}
	std::string cpu_info = CPUBrandString;
	std::cout << "--PROC INFO--" << std::endl;
	std::cout << cpu_info << std::endl;
	return cpu_info;
}

void collector::create_root_folder() {
	std::cout << "--CREATE DIR--" << std::endl;
	if (!fs::is_directory(this->root_folder_) || !fs::exists(this->root_folder_)) {
		fs::create_directory(this->root_folder_);
		std::cout << "--CREATED::" << this->root_folder_ << std::endl;
		return;
	}
	std::cout << "--CREATED::" << this->root_folder_ << "::--" << std::endl;
}

void collector::move_collected_to_root() {
	std::cout << "--COPY FILES TO DIR--" << std::endl;
	for (auto& ptr : this->files_to_copy_in_root_) {
		std::ifstream src_file(ptr, std::ios::binary);
		std::string only_file_name = ptr.substr(ptr.find_last_of('\\'));
		std::string dest_file_path = this->root_folder_ + only_file_name;
		std::ofstream dest_file(dest_file_path, std::ios::binary);
		dest_file << src_file.rdbuf();
		src_file.close();
		dest_file.close();
	}
	//this part is delete ./log.txt after copy in root folder
	try {
		std::filesystem::remove(this->files_to_copy_in_root_[0]);
	}
	catch (const std::filesystem::filesystem_error& error) {
		std::cerr << error.what() << '\n';
	}
	
}

void collector::get_pc_disk_space() {
	std::cout << "--DISK INFO--" << std::endl;
	std::vector<std::string> disklist;
	DWORD drives = GetLogicalDrives();
	for (int ptr = 0; ptr < 26; ptr++) {
		if ((drives & (1 << ptr))) {
			TCHAR drvname[] = { TEXT('A') + ptr,TEXT(':'),TEXT('\\'),TEXT('\0') };
			std::wstring tmp_tch_to_str(&drvname[0]);
			std::string tmp_str(tmp_tch_to_str.begin(), tmp_tch_to_str.end());
			disklist.push_back(tmp_str);
			tmp_tch_to_str.clear();
			tmp_str.clear();
		}
	}
	for (auto& D : disklist) {
		fs::space_info currVol = fs::space(D);
		if (currVol.available) {
			std::string cv = "Disk " + D + " | " + "Capacity: " + std::to_string(currVol.capacity / 1024 / 1024) + " Mb | Free: " + std::to_string(currVol.free / 1024 / 1024) + " Mb | Available: " + std::to_string(currVol.available / 1024 / 1024) + " Mb |";
			this->main_log_container_->add_log_string(cv);

			std::cout << cv << std::endl;
		}
		
	}
}

std::string collector::get_pc_ram_info() {
	std::cout << "--RAM INFO--" << std::endl;
	MEMORYSTATUSEX memInfo;
	memInfo.dwLength = sizeof(MEMORYSTATUSEX);
	GlobalMemoryStatusEx(&memInfo);
	DWORDLONG totalPhysMem = memInfo.ullTotalPhys;
	std::string total_ram = "Physical RAM: " + std::to_string(totalPhysMem / 1024 / 1024) + " Mb";
	std::cout << "Physical RAM: " << totalPhysMem /1024/1024 << " MB" << std::endl;
	return total_ram;
}

void collector::get_pc_motherboard_info() {
	
}

void collector::collect_log_file(){

	try {
		for (const auto& ptrD : place) {
			for (const auto& ptrM : mask) {
				for (const auto& ptrF : fs::recursive_directory_iterator(ptrD)) {
					std::string tmpF = ptrF.path().string();
					if (tmpF.find(ptrM) != std::string::npos) {
						this->files_to_copy_in_root_.push_back(tmpF);
					}
					tmpF.clear();
				}
			}
		}
	}
	catch(fs::filesystem_error &err){
		std::cerr << err.what() << std::endl;
		this->main_log_container_->add_log_string(err.what());
	}
}