#pragma once
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <iostream>
#include <iomanip>
using namespace std;


//func declar
void hidecursor();
void UNhidecursor();
void man_menu_view();
void help_menu_view();
//display main menu
void man_menu_view() {
	hidecursor();
	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	cout << endl << "***|";
	SetConsoleTextAttribute(hConsole, BACKGROUND_RED | BACKGROUND_BLUE | BACKGROUND_GREEN | 5);
	cout << "Poltavskiy Nick log collector";
	SetConsoleTextAttribute(hConsole, 7);
	cout << endl << "***|";
	SetConsoleTextAttribute(hConsole, BACKGROUND_RED | BACKGROUND_BLUE | BACKGROUND_GREEN | 5);
	cout << "Help menu";
	SetConsoleTextAttribute(hConsole, 7);
	SetConsoleTextAttribute(hConsole, 8);
	cout << endl << "***|" << "Poltavskiy Nick logger(1)";
	SetConsoleTextAttribute(hConsole, 7);
	SetConsoleTextAttribute(hConsole, 8);
	cout << endl << "***|" << "SYNOPSIS:";
	SetConsoleTextAttribute(hConsole, 7);
	cout << endl << "***|" << "		logger.exe ";
	cout << endl << "***|" << "		logger.exe [OPTION]";
	SetConsoleTextAttribute(hConsole, 8);
	cout << endl << "***|" << "DESCRIPTION:";
	SetConsoleTextAttribute(hConsole, 7);
	cout << endl << "***|" << "		logger.exe default takes only local workstation info and formed folder with %pc_name%.txt log file";
	cout << endl << "***|" << "		logger.exe /a besides local info, copy system log from winevnt dir(NEED ADMIN RIGTH) - UUPSY this func. is not availible now";
	cout << endl << "***|" << "		logger.exe /?(help menu)";
	cout << endl << "***|" << "		logger.exe /man(man menu)";
	SetConsoleTextAttribute(hConsole, BACKGROUND_RED | BACKGROUND_BLUE | BACKGROUND_GREEN | 9);
	cout << endl << "...by Dies_Irae" << endl;
	cout << endl;
	cout << endl;
	cout << endl;
	cout << endl;
	cout << endl;
	cout << endl;
	SetConsoleTextAttribute(hConsole, 7);
	UNhidecursor();
}

void help_menu_view() {
	cout << "	Usage: logger.exe" << endl << endl;
	cout << "	OPTIONS: " << endl;
	cout << "		a - key to copy all sys log - UUPSY this func. is not availible now" << endl;
	cout << "		m - man menu" << endl;
	cout << "		? - help menu" << endl << endl;
	cout << "	EXAMPLES: " << endl;
	cout << "		logger.exe" << endl;
	cout << "		logger.exe /a - UUPSY this func. is not availible now" << endl;
	cout << "		logger.exe /h" << endl;
	cout << "		logger.exe /?" << endl;
}

void hidecursor() {
	HANDLE consoleHandle = GetStdHandle(STD_OUTPUT_HANDLE);
	CONSOLE_CURSOR_INFO info;
	info.dwSize = 100;
	info.bVisible = FALSE;
	SetConsoleCursorInfo(consoleHandle, &info);
}
void UNhidecursor() {
	HANDLE consoleHandle = GetStdHandle(STD_OUTPUT_HANDLE);
	CONSOLE_CURSOR_INFO info;
	info.bVisible = TRUE;
	SetConsoleCursorInfo(consoleHandle, &info);

}