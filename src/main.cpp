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
		std::string path = argv[1];
		HttpServer server(8888, path);

		while(true) {
			struct sockaddr_storage incoming_addr;
			socklen_t incoming_addr_size = sizeof(incoming_addr);
			int incoming_fd = accept(server.getServerSocket(), (struct sockaddr*) &incoming_addr, &incoming_addr_size);
			if(incoming_fd == -1) {
				throw std::runtime_error(std::string("Accept error: ") + strerror(errno));
			}

			std::vector<char> buffer(4096);
			read(incoming_fd, buffer.data(), buffer.size());
			std::string request(buffer.begin(), buffer.end());

			HttpHeader header = server.parseRequest(request);
			if(header.method == "GET") {
				std::string response_header = "HTTP/1.0 200 OK\nContent-type: text/html\n\n";
				std::string response_body = server.createResponse(header.uri);
				std::string response = response_header + response_body;
				send(incoming_fd, response.c_str(), response.length(), 0);
			}
			close(incoming_fd);
		}
	}
	catch(std::exception& e) {
		std::cout << e.what() << std::endl;
	}
	return 0;
}