#pragma once
#define WIN32_LEAN_AND_MEAN
#include <vector>
#include <string>
//files to collect and places where collect
std::vector<std::string> place{"%SystemRoot%\\System32\\winevt\\Logs\\" , "%SystemRoot%\\Sysnative\\winevt\\Logs\\"};

std::vector<std::string> mask{".evtx", ".log", ".txt"};

char restricted[] = {' ', '<', '>', ':', '"', '/', '\\', '|', '?', '*'};
