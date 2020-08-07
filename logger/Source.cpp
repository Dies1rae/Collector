#include <iostream>
#include <string>
#include "collector.h"
#include "logg.h"

int main(int argc, char* argv[]) {
	//parse the arg and start main loop
	char che[18] = "LOGGING END";
	char chb[20] = "LOGGING BEGIN";
	collector info_coll;
	logg* main_l = new logg();
	info_coll.init(main_l);
	main_l->add_log_string_timemark_(chb);
	main_l->write_to_file();
	info_coll.run();
	main_l->write_to_file();
	main_l->add_log_string_timemark_(che);
	main_l->write_to_file();
	return 0;
}