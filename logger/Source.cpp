#include <iostream>
#include <string>
#include "collector.h"
#include "logg.h"

int main(int argc, char* argv[]) {
	//parse the arg and start main loop

	collector info_coll;
	logg* main_l = new logg();
	info_coll.init(main_l);
	info_coll.run();
	return 0;
}