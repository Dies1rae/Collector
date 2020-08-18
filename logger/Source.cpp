#include <iostream>
#include <string>
#include "collector.h"
#include "logg.h"
#include "Menu.h"
#define WIN32_LEAN_AND_MEAN

int main(int argc, char* argv[]) {
	//parse the arg and start main loop
	if (argc == 1) {
		collector info_coll;
		logg* main_l = new logg();
		info_coll.init(main_l,false);
		info_coll.run();
	}
	if (argc == 2 && (argv[1][0] == '/' && argv[1][1] == 'm')) {
		man_menu_view();
	}
	if (argc == 2 && (argv[1][0] == '/' && argv[1][1] == '?')) {
		help_menu_view();
	}
	if (argc == 2 && (argv[1][0] == '/' && argv[1][1] == 'a')) {
		collector info_coll;
		logg* main_l = new logg();
		info_coll.init(main_l, true);
		info_coll.run();
	}
	return 0;
}