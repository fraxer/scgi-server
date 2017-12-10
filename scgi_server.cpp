#include "lib/scgi.hpp"

using namespace std;

int main(int argc, char **argv) {
	locale system("");
    locale::global(system);

	//If You started server as daemon You must to use demonize() method 
	if (demonize() < 1) {
		return 0;
	}

	init((char*)"127.0.0.1", 8080);

	return 0;
}