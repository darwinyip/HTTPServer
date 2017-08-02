#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <cstring>
#include <unistd.h>
#include <cerrno>
#include <netdb.h>
#include <stdexcept>
#include <sstream>
#include <map>
#include <iterator>
#include "server.h"

int main(int argc, char const *argv[])
{
	try {
		HttpServer server(8888);

		struct sockaddr_storage incoming_addr;
		socklen_t incoming_addr_size = sizeof(incoming_addr);
		int incoming_fd = accept(server.getServerSocket(), (struct sockaddr*) &incoming_addr, &incoming_addr_size);

		if(incoming_fd == -1) {
			throw std::runtime_error(std::string("Accept error: ") + strerror(errno));
		}

		std::vector<char> buffer(4096);

		read(incoming_fd, buffer.data(), buffer.size());

		HttpHeader header = server.parseRequest(buffer);

		if(header.method == "GET") {
			std::string message = "HTTP/1.0 200 OK\nContent-type: text/html\n\n<H1>Success</H1>\n";
			send(incoming_fd, message.c_str(), message.length(), 0);
		}
	}
	catch(std::exception& e) {
		std::cout << e.what() << std::endl;
	}
	return 0;
}