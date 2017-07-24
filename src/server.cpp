#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <cstring>
#include <iostream>
#include <unistd.h>
#include <errno.h>
#include <netdb.h>
#include <stdexcept>
#include <vector>
#include <sstream>
#include <map>
#include <iterator>

class HttpServer {
public:
	HttpServer(int port);
	~HttpServer();
	int getServerSocket();

private:
	int server_socket_fd;
};

HttpServer::HttpServer(int port) {
	struct addrinfo hints, *res;
	std::memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = INADDR_ANY;

	int ai = getaddrinfo(NULL, std::to_string(port).c_str(), &hints, &res);
	if(ai != 0) {
		throw std::runtime_error(std::string("getaddrinfo error: ") + gai_strerror(ai));
	}

	if((this->server_socket_fd = socket(res->ai_family, res->ai_socktype, res->ai_protocol)) == -1) {
		throw std::runtime_error(std::string("Socket error: ") + strerror(errno));
	}

	if(bind(this->server_socket_fd, res->ai_addr, res->ai_addrlen) == -1) {
		throw std::runtime_error(std::string("Binding error: ") + strerror(errno));
	}

	if(listen(this->server_socket_fd, 10) == -1) {
		throw std::runtime_error(std::string("Listening error: ") + strerror(errno));
	}
}

HttpServer::~HttpServer() {}

int HttpServer::getServerSocket() {
	return this->server_socket_fd;
}

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

		std::istringstream request(buffer.data()); 
		std::string line;
		while(std::getline(request, line)) {
			if(line.empty()) break;
			// One-line tokenizing string
			// https://stackoverflow.com/a/237280/1061193
			std::istringstream iss(line);
			std::vector<std::string> method{std::istream_iterator<std::string>{iss}, std::istream_iterator<std::string>{}};

			if(method[0] == "GET") {
				std::string message = "HTTP/1.0 200 OK\nContent-type: text/html\n\n<H1>Success</H1>\n";
				send(incoming_fd, message.c_str(), message.length(), 0);
			}
		}
	}
	catch(std::exception& e) {
		std::cout << e.what() << std::endl;
	}
	return 0;
}