#include "collector.h"
#include "places.h"
#include <string>
#include <iostream>
#include <WinBase.h>
#include <Windows.h>
#include <fstream>
#include <sstream>
#include <intrin.h>
#include <vector>
#include <sys/stat.h>
#include <conio.h> 
#include <sys/types.h> 
#include <iphlpapi.h>
#include <stdio.h>
#include <stdlib.h> 
#include <direct.h> 
#include <WinSock2.h>
#include <lm.h>

#pragma comment(lib, "IPHLPAPI.lib")
#pragma comment(lib,"user32.lib")
#pragma comment(lib, "ws2_32.lib")
#pragma comment(lib, "netapi32.lib")


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
	this->main_log_container_->add_log_string("");
	this->main_log_container_->add_log_string("Started by: " + get_pc_user_());
	this->main_log_container_->add_log_string("---***HARDWARE INFO***---");
	std::string pc_name = get_pc_name();
	//set property txt file name
	this->main_log_container_->set_logpath(".\\Log_" + pc_name + ".txt");
	this->files_to_copy_in_root_.push_back(this->main_log_container_->get_log_path());
	//-----
	this->root_folder_ += pc_name+'\\';
	this->main_log_container_->add_log_string(pc_name);
	this->main_log_container_->add_log_string(get_OS_win_ver_());
	this->main_log_container_->add_log_string(get_pc_CPU_info());
	this->main_log_container_->add_log_string(get_pc_ram_info());
	this->main_log_container_->add_log_string(get_pc_motherboard_info());
	this->main_log_container_->add_log_string(get_pc_videodev_info());
	get_pc_disk_space();
	for (const auto& ptr : get_pc_network_hard_info()) {
		this->main_log_container_->add_log_string(ptr.first + "||" + ptr.second);
	}
	this->main_log_container_->add_log_string(get_pc_network_soft_addr_info());
	this->main_log_container_->add_log_string(get_pc_network_soft_info());
	this->main_log_container_->add_log_string("");
	this->main_log_container_->add_log_string("---***Users %homepaths%***---");
	for (const auto& ptr : get_pc_user_list_()) {
		this->main_log_container_->add_log_string(ptr);
	}

	this->main_log_container_->add_log_string("");
	this->main_log_container_->add_log_string("---***SOFTWARE INFO***---");
	for (const auto& ptr : get_installed_software()) {
		this->main_log_container_->add_log_string(ptr);
	}

	if (this->key_) {
		this->collect_log_file();
		std::cout << "copy done" << std::endl;
	}
	this->main_log_container_->add_log_string("");
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
		for (auto& ch : CompN) {
			if (C == ch) {
				ch = '_';
			}
		}
	}
	return CompN;
}
std::string collector::get_OS_win_ver_() {
	DWORD Buff_Prod_name_ = 1024, Buff_rel = 1024;
	WCHAR Prod_name_[512], Rel_[512];
	RegGetValue(HKEY_LOCAL_MACHINE, L"SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion", L"ReleaseId", RRF_RT_ANY, NULL, Rel_, &Buff_rel);
	RegGetValue(HKEY_LOCAL_MACHINE, L"SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion", L"ProductName", RRF_RT_ANY, NULL, Prod_name_, &Buff_Prod_name_);
	std::wstring Prod_name_tmp(Prod_name_);
	std::wstring Rel_tmp(Rel_);
	std::wstring all_tmp_w = L"Windows version: " + Prod_name_tmp + L" | Rel: " + Rel_tmp + L" | ";
	std::string OS_WIN_VER(all_tmp_w.begin(), all_tmp_w.end());
	UINT x64test = GetSystemWow64DirectoryA(NULL, 0);
	if (GetLastError() == ERROR_CALL_NOT_IMPLEMENTED) {
		OS_WIN_VER += "Type: x32 |";
	}
	else {
		OS_WIN_VER += "Type: x64 |";
	}
	return OS_WIN_VER;
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
	int status_dir_ = _mkdir(this->root_folder_.c_str());
	this->move_collected_to_root();
	std::remove(this->files_to_copy_in_root_[0].c_str());
}

void collector::move_collected_to_root() {
	for (auto& ptr : this->files_to_copy_in_root_) {
		std::string dest_file_path = this->root_folder_ + ptr.substr(ptr.find_last_of('\\'));
		std::ifstream src(ptr, std::ios::binary);
		std::ofstream dst(dest_file_path, std::ios::binary);
		dst << src.rdbuf();
		src.close();
		dst.close();
	}
}

void collector::get_pc_disk_space() {
	//disklist
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
	//diskspace
	for (const std::string& D : disklist) {		
		BOOL diskRes_;
		unsigned __int64 i64FreeBytesToCaller, i64TotalBytes, i64FreeBytes;
		std::wstring d_temp_ = std::wstring(D.begin(), D.end());
		LPCWSTR tmp_disk_letter = d_temp_.c_str();
		diskRes_ = GetDiskFreeSpaceEx(tmp_disk_letter, (PULARGE_INTEGER)&i64FreeBytesToCaller, (PULARGE_INTEGER)&i64TotalBytes, (PULARGE_INTEGER)&i64FreeBytes);
		if (diskRes_) {
			std::string cv = "Disk " + D + " | " + "Capacity: " + std::to_string(i64TotalBytes / (1024 * 1024)) + " Mb | Free: " + std::to_string(i64FreeBytes / (1024 * 1024)) + " Mb | Available: " + std::to_string(i64FreeBytesToCaller / (1024 * 1024)) + " Mb |";
			this->main_log_container_->add_log_string(cv);
		}
	}
	//disk_serial
	WCHAR disk_mod[512];
	DWORD Buff_ = 512;
	RegGetValue(HKEY_LOCAL_MACHINE, L"SYSTEM\\CurrentControlSet\\Services\\disk\\Enum", L"0", RRF_RT_ANY, NULL, disk_mod, &Buff_);
	std::wstring dsk_mod_tmp(disk_mod);
	std::string res_dsk_mod_tmp(dsk_mod_tmp.begin(), dsk_mod_tmp.end());
	res_dsk_mod_tmp = res_dsk_mod_tmp.substr(res_dsk_mod_tmp.find_first_of("_")+1, res_dsk_mod_tmp.find_last_of("\\") - res_dsk_mod_tmp.find_first_of("_")+1);
	res_dsk_mod_tmp.insert(0, "System drive model: ");
	this->main_log_container_->add_log_string(res_dsk_mod_tmp);
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
		if (find) {
			break;
		}
		//and here we get trouble, if you dont have any VID card on workstation, you got problem with no for circle out....))) 
	}
	std::wstring video_tmp = vid.DeviceString;
	std::string video(video_tmp.begin(), video_tmp.end());
	return video;
}

std::vector<std::pair<std::string, std::string>> collector::get_pc_network_hard_info() {
	std::vector<std::pair<std::string, std::string>> net_ada_info;
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
	}
	PIP_ADAPTER_INFO pAdapter = dad_info;
	while (pAdapter) {
		//get MAC part
		std::stringstream stream_mac;
		for (int i = 0; i < pAdapter->AddressLength; i++) {
			if (i == (pAdapter->AddressLength - 1)) {
				stream_mac << std::hex << (int)pAdapter->Address[i];
			}
			else {
				stream_mac << std::hex << (int)pAdapter->Address[i];
				stream_mac << '-';
			}
		}
		std::string tmp_mac(stream_mac.str());
		//-----------
		std::string tmp_net_descr = pAdapter->Description;
		tmp_net_descr += " | ";
		tmp_net_descr += pAdapter->AdapterName;
		net_ada_info.push_back(std::make_pair(tmp_net_descr, tmp_mac));
		tmp_net_descr.clear();
		pAdapter = pAdapter->Next;
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
	pFixedInfo = (FIXED_INFO*)malloc(sizeof(FIXED_INFO));
	if (pFixedInfo == NULL) {
		std::cerr << "ERROR ALLOC MEMORY" << std::endl;
		net_ada_info += "|ERROR ALLOC MEMORY|";
	}
	ulOutBufLen_ = sizeof(FIXED_INFO);
	if (GetNetworkParams(pFixedInfo, &ulOutBufLen_) == ERROR_BUFFER_OVERFLOW) {
		free(pFixedInfo);
		pFixedInfo = (FIXED_INFO*)malloc(ulOutBufLen_);
		if (pFixedInfo == NULL) {
			std::cerr << "Error allocating memory needed to call GetNetworkParams" << std::endl;
			net_ada_info += "|Error allocating memory needed to call GetNetworkParams|";
		}
	}
	if (dwRetVa_= GetNetworkParams(pFixedInfo, &ulOutBufLen_) == NO_ERROR) {
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

std::string collector::get_pc_network_soft_addr_info() {
	std::string addr_info;
	WSADATA Data;
	int wsa_ret = WSAStartup(0x101, &Data);
	struct hostent* loc = gethostbyname(get_pc_network_soft_info().c_str());
	if (loc == NULL) {
		std::cerr << "Gethostbyname() failed" << std::endl;
		addr_info += "|Gethostbyname() failed|";
	}
	else {
		unsigned int ptr = 0;
		while (loc->h_addr_list[ptr] != NULL) {
			addr_info += inet_ntoa(*(struct in_addr*)(loc->h_addr_list[ptr]));
			addr_info += '\n';
			ptr++;
		}
	}
	return addr_info;
}

std::vector<std::string> collector::get_installed_software() {
	std::vector<std::string> res;
	HKEY hUninstKey = NULL;
	HKEY hAppKey = NULL;
	WCHAR sAppKeyName[1024];
	WCHAR sSubKey[1024];
	WCHAR sDisplayName[1024];
	long lResult = ERROR_SUCCESS;
	DWORD dwType = KEY_ALL_ACCESS;
	DWORD dwBufferSize = 0;

	//Open the "Uninstall" key.
	if (RegOpenKeyEx(HKEY_LOCAL_MACHINE, L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\", 0, KEY_READ, &hUninstKey) != ERROR_SUCCESS){
		std::cerr << "OOPSY" << std::endl;
		res.push_back("OOPSY");
	}

	for (DWORD dwIndex = 0; lResult == ERROR_SUCCESS; dwIndex++){
		//Enumerate all sub keys...
		dwBufferSize = sizeof(sAppKeyName);
		if ((lResult = RegEnumKeyEx(hUninstKey, dwIndex, sAppKeyName,&dwBufferSize, NULL, NULL, NULL, NULL)) == ERROR_SUCCESS){
			//Open the sub key.
			wsprintf(sSubKey, L"%s\\%s", L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\", sAppKeyName);
			if (RegOpenKeyEx(HKEY_LOCAL_MACHINE, sSubKey, 0, KEY_READ, &hAppKey) != ERROR_SUCCESS){
				RegCloseKey(hAppKey);
				RegCloseKey(hUninstKey);
				std::cerr << "OOPSY" << std::endl;
				res.push_back("OOPSY");
			}

			//Get the display name value from the application's sub key.
			dwBufferSize = sizeof(sDisplayName);
			if (RegQueryValueEx(hAppKey, L"DisplayName", NULL,&dwType, (unsigned char*)sDisplayName, &dwBufferSize) == ERROR_SUCCESS){
				//wprintf(L"%s\n", sDisplayName);
				std::wstring tmp_disp_name = sDisplayName;
				std::string tmp_disp_name_input(tmp_disp_name.begin(), tmp_disp_name.end());
				res.push_back(tmp_disp_name_input);
			}
			else {
				//Display name value doe not exist, this application was probably uninstalled.
			}
			RegCloseKey(hAppKey);
		}
	}
	RegCloseKey(hUninstKey);
	return res;
}

std::string collector::get_pc_user_() {
	TCHAR buff_[32767];
	DWORD buffer_ch_ctr_ = 32767;
	if (!GetUserName(buff_, &buffer_ch_ctr_)) {
		std::cerr << "Get_username error" << std::endl;
		std::string res = "Get_username error";
		return res;
	}
	std::wstring tmp_ = buff_;
	std::string res(tmp_.begin(), tmp_.end());
	return res;
}
std::vector<std::string> collector::get_pc_user_list_() {
	std::vector<std::string> res;

	HKEY hUserKey = NULL;
	HKEY hSubUserKey = NULL;
	WCHAR sUserKeyName[1024];
	WCHAR sSubKey[1024];
	WCHAR sDisplayName[1024];
	long lResult = ERROR_SUCCESS;
	DWORD dwType = KEY_ALL_ACCESS;
	DWORD dwBufferSize = 0;

	//Open the "Uninstall" key.
	if (RegOpenKeyEx(HKEY_LOCAL_MACHINE, L"SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion\\ProfileList\\", 0, KEY_READ, &hUserKey) != ERROR_SUCCESS) {
		std::cerr << "OOPSY" << std::endl;
		res.push_back("OOPSY");
	}

	for (DWORD dwIndex = 0; lResult == ERROR_SUCCESS; dwIndex++) {
		//Enumerate all sub keys...
		dwBufferSize = sizeof(sUserKeyName);
		if ((lResult = RegEnumKeyEx(hUserKey, dwIndex, sUserKeyName, &dwBufferSize, NULL, NULL, NULL, NULL)) == ERROR_SUCCESS) {
			//Open the sub key.
			wsprintf(sSubKey, L"%s\\%s", L"SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion\\ProfileList\\", sUserKeyName);
			if (RegOpenKeyEx(HKEY_LOCAL_MACHINE, sSubKey, 0, KEY_READ, &hSubUserKey) != ERROR_SUCCESS) {
				RegCloseKey(hSubUserKey);
				RegCloseKey(hUserKey);
				std::cerr << "OOPSY" << std::endl;
				res.push_back("OOPSY");
			}

			//Get the display name value from the application's sub key.
			dwBufferSize = sizeof(sDisplayName);
			if (RegQueryValueEx(hSubUserKey, L"ProfileImagePath", NULL, &dwType, (unsigned char*)sDisplayName, &dwBufferSize) == ERROR_SUCCESS) {
				//wprintf(L"%s\n", sDisplayName);
				std::wstring tmp_disp_name = sDisplayName;
				std::string tmp_disp_name_input(tmp_disp_name.begin(), tmp_disp_name.end());
				for (size_t ptr = 0; ptr < tmp_disp_name_input.size(); ptr++) {
					if (tmp_disp_name_input[ptr] >= 65 && tmp_disp_name_input[ptr] <= 90) {
						tmp_disp_name_input[ptr] += 32;
					}
				}
				res.push_back(tmp_disp_name_input);
			}
			else {
				//Display name value doe not exist, this application was probably uninstalled.
			}
			RegCloseKey(hSubUserKey);
		}
	}
	RegCloseKey(hUserKey);

	return res;
}

void collector::collect_log_file(){
	for (const auto& folder : place) {
		std::string cpy_cmd = "copy " + folder + " .\\logs\\";
		_mkdir(".\\logs\\");
		system(cpy_cmd.c_str());
	}
}
