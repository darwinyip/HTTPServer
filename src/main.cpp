#include "server.h"

int main(int argc, char const *argv[])
{
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