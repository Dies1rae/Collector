#include <iostream>
#include <string>
#include "collector.h"
#include "logg.h"

int main(int argc, char* argv[]) {
	//parse the arg and start main loop
	std::cout << "--BEGIN--" << std::endl;
	collector info_coll;
	logg* main_l = new logg();
	info_coll.init(main_l);
	info_coll.run();
	std::cout << "--END--" << std::endl;
	return 0;
}