#include "server.h"

int main(int argc, char const *argv[])
{
	if(argc != 2) {
		std::cout << "Source directory required." << std::endl;
		return 1;
	}
	try {
		std::string path = argv[1];
		HttpServer server(8888, path);
		server.serve();
	}
	catch(std::exception& e) {
		std::cout << e.what() << std::endl;
	}
	return 0;
}