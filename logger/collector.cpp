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

#include <iphlpapi.h>
#include <stdio.h>

#pragma comment(lib, "IPHLPAPI.lib")
#pragma comment(lib,"user32.lib")
namespace fs = std::filesystem;

void collector::init(logg* L,bool K) {
	this->main_log_container_ = L;
	char ch[14] = "INIT DONE";
	this->main_log_container_->add_log_string(ch);
	this->key_ = K;
}

void collector::run() {
	char che[18] = "LOGGING END";
	char chb[20] = "LOGGING BEGIN";
	this->main_log_container_->add_log_string_timemark_(chb);
	this->main_log_container_->add_log_string("---HARDWARE INFO---");
	std::string pc_name = get_pc_name();
	//set property txt file name
	this->main_log_container_->set_logpath(".\\Log_" + pc_name+".txt");
	this->files_to_copy_in_root_.push_back(this->main_log_container_->get_log_path());
	//-----
	this->root_folder_ += pc_name+'\\';
	this->main_log_container_->add_log_string(pc_name);
	this->main_log_container_->add_log_string(get_pc_CPU_info());
	this->main_log_container_->add_log_string(get_pc_ram_info());
	this->main_log_container_->add_log_string(get_pc_motherboard_info());
	this->main_log_container_->add_log_string(get_pc_videodev_info());
	get_pc_disk_space();
	this->main_log_container_->add_log_string(get_pc_network_hard_info());
	this->main_log_container_->add_log_string(get_pc_network_soft_info());


	if (this->key_) {
		this->collect_log_file();
	}
	this->main_log_container_->add_log_string("");
	this->main_log_container_->add_log_string_timemark_(che);
	this->main_log_container_->write_to_file();
	this->create_root_folder_and_move_logs();
}
std::string collector::get_pc_name() {
	TCHAR computerName[MAX_COMPUTERNAME_LENGTH + 1];
	DWORD size = sizeof(computerName) / sizeof(computerName[0]);
	GetComputerName(computerName, &size);
	std::wstring tmp = computerName;
	std::string CompN(tmp.begin(), tmp.end());
	for (const char C : restricted) {
		std::replace(CompN.begin(), CompN.end(), C, '_');
	}

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
	return cpu_info;
}

void collector::create_root_folder_and_move_logs() {
	if (!fs::is_directory(this->root_folder_) || !fs::exists(this->root_folder_)) {
		fs::create_directory(this->root_folder_);
	}
	this->move_collected_to_root();
	std::filesystem::remove(this->files_to_copy_in_root_[0]);
}

void collector::move_collected_to_root() {
	for (auto& ptr : this->files_to_copy_in_root_) {
		std::string only_file_name = ptr.substr(ptr.find_last_of('\\'));
		std::string dest_file_path = this->root_folder_ + only_file_name;
		fs::copy_file(ptr, dest_file_path,fs::copy_options::overwrite_existing);
	}
	
}

void collector::get_pc_disk_space() {
	std::vector<std::string> disklist;
	DWORD drives = GetLogicalDrives();
	for (int i = 0; i < 26; i++){
		if ((drives & (1 << i))){
			wchar_t driveName[] = { L'A' + i, L':', L'\\', L'\0' };
			if (GetDriveType(driveName) == 3 || GetDriveType(driveName) == 4) {
				std::wstring ws_tmp(driveName);
				std::string ws_str_tmp(ws_tmp.begin(), ws_tmp.end());
				disklist.push_back(ws_str_tmp);
			}
		}
	}

	for (const std::string& D : disklist) {		
		fs::space_info currVol = fs::space(D);
		std::string cv = "Disk " + D + " | " + "Capacity: " + std::to_string(currVol.capacity / 1024 / 1024) + " Mb | Free: " + std::to_string(currVol.free / 1024 / 1024) + " Mb | Available: " + std::to_string(currVol.available / 1024 / 1024) + " Mb |";
		this->main_log_container_->add_log_string(cv);
	}
}

std::string collector::get_pc_ram_info() {
	MEMORYSTATUSEX memInfo;
	memInfo.dwLength = sizeof(MEMORYSTATUSEX);
	GlobalMemoryStatusEx(&memInfo);
	DWORDLONG totalPhysMem = memInfo.ullTotalPhys;
	std::string total_ram = "Physical RAM: " + std::to_string(totalPhysMem / 1024 / 1024) + " Mb";
	return total_ram;
}

std::string collector::get_pc_motherboard_info() {
	WCHAR manuf[512], ModProduct[512];
	DWORD Buff1 = 1024, Buff2 = 1024;
	RegGetValue(HKEY_LOCAL_MACHINE, L"HARDWARE\\DESCRIPTION\\System\\BIOS", L"BaseBoardManufacturer", RRF_RT_ANY, NULL, manuf, &Buff1);
	RegGetValue(HKEY_LOCAL_MACHINE, L"HARDWARE\\DESCRIPTION\\System\\BIOS", L"BaseBoardProduct", RRF_RT_ANY, NULL, ModProduct, &Buff2);
	std::wstring manufact_tmp(manuf);
	std::wstring ModProd_tmp(ModProduct);
	std::wstring all_tmp_w = L"Manufacture: " + manufact_tmp + L" | Model: " + ModProd_tmp + L" |";
	std::string res_mainB(all_tmp_w.begin(), all_tmp_w.end());
	return res_mainB;
}

std::string collector::get_pc_videodev_info() {
	DISPLAY_DEVICE vid = { sizeof(vid), 0 };
	for (int ptr = 0;;ptr++) {
		BOOL find = EnumDisplayDevices(NULL, ptr, &vid, EDD_GET_DEVICE_INTERFACE_NAME);
		if (!find) {
			break;
		}
	}
	std::wstring video_tmp = vid.DeviceString;
	std::string video(video_tmp.begin(), video_tmp.end());
	return video;
}

std::string collector::get_pc_network_hard_info() {
	std::string net_ada_info;
	IP_ADAPTER_INFO* dad_info;
	dad_info = (IP_ADAPTER_INFO*)malloc(sizeof(IP_ADAPTER_INFO));
	ULONG ulOutBufLen;
	ulOutBufLen = sizeof(IP_ADAPTER_INFO);
	DWORD dwRetVal;
	if (GetAdaptersInfo(dad_info, &ulOutBufLen) != ERROR_SUCCESS) {
		free(dad_info);
		dad_info = (IP_ADAPTER_INFO*)malloc(ulOutBufLen);
	}
	if ((dwRetVal = GetAdaptersInfo(dad_info, &ulOutBufLen)) != ERROR_SUCCESS) {
		std::cerr << "GetAdaptersInfo call failed with "<< dwRetVal << std::endl;
		system("PAUSE");
	}
	PIP_ADAPTER_INFO pAdapter = dad_info;
	if (pAdapter) {
		std::string tmp_net_descr = pAdapter->Description;
		net_ada_info += tmp_net_descr;
		net_ada_info += " | ";
		net_ada_info += pAdapter->AdapterName;
	}
	if (dad_info) {
		free(dad_info);
	}
	return net_ada_info;
}

std::string collector::get_pc_network_soft_info() {
	std::string net_ada_info;
	FIXED_INFO* pFixedInfo;
	ULONG ulOutBufLen_;
	DWORD dwRetVa_;
	IP_ADDR_STRING* pIPAddr;
	pFixedInfo = (FIXED_INFO*)malloc(sizeof(FIXED_INFO));
	if (pFixedInfo == NULL) {
		std::cerr << "ERROR ALLOC MEMORY" << std::endl;
		system("PAUSE");
	}
	ulOutBufLen_ = sizeof(FIXED_INFO);
	if (GetNetworkParams(pFixedInfo, &ulOutBufLen_) == ERROR_BUFFER_OVERFLOW) {
		free(pFixedInfo);
		pFixedInfo = (FIXED_INFO*)malloc(ulOutBufLen_);
		if (pFixedInfo == NULL) {
			std::cerr << "Error allocating memory needed to call GetNetworkParams" << std::endl;
			system("PAUSE");
		}
	}
	if (dwRetVa_ = GetNetworkParams(pFixedInfo, &ulOutBufLen_) == NO_ERROR) {
		std::string tmp_net_name_ = pFixedInfo->HostName;
		tmp_net_name_ +=".";
		tmp_net_name_ += pFixedInfo->DomainName;
		net_ada_info += tmp_net_name_;
	}
	if (pFixedInfo) {
		free(pFixedInfo);
	}
	return net_ada_info;
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
		system("PAUSE");
	}
}