#include "collector.h"
#include "places.h"
#include <filesystem>
#include <string>
#include <iostream>
#include <WinBase.h>
#include <Windows.h>

namespace fs = std::filesystem;

void collector::init(logg* L) {
	this->main_log_container_ = L;
	char ch[14] = "INIT DONE";
	this->main_log_container_->add_log_string(ch);
}

void collector::run() {
	this->main_log_container_->add_log_string("---MAIN INFO---");
	this->main_log_container_->add_log_string(get_pc_name());
	this->main_log_container_->add_log_string(get_pc_sys_info());


	this->main_log_container_->add_log_string("");
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