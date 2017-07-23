#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <cstring>
#include <iostream>
#include <unistd.h>
#include <errno.h>
#include <netdb.h>
#include <stdexcept>

class HttpServer {
public:
	HttpServer();
	~HttpServer();
	bool start(int port);
	int getServerSocket();

private:
	int server_socket_fd;
};

HttpServer::HttpServer() {}

HttpServer::~HttpServer() {}

bool HttpServer::start(int port) {
	struct addrinfo hints, *res;
	std::memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = INADDR_ANY;

	getaddrinfo(NULL, std::to_string(port).c_str(), &hints, &res);

	this->server_socket_fd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);

	if(this->server_socket_fd == -1) {
		std::cerr << "Socket error: " << strerror(errno);
		return EXIT_FAILURE;
	}

	if(bind(this->server_socket_fd, res->ai_addr, res->ai_addrlen) == -1) {
		std::cerr << "Binding error: " << strerror(errno);
		return EXIT_FAILURE;
	}

	if(listen(this->server_socket_fd, 10) == -1) {
		std::cerr << "Listening error: " << strerror(errno);
		return EXIT_FAILURE;
	}
	return EXIT_SUCCESS;
}

int HttpServer::getServerSocket() {
	return this->server_socket_fd;
}

int main(int argc, char const *argv[])
{

	HttpServer server;
	server.start(8888);

	struct sockaddr_storage incoming_addr;


	socklen_t incoming_addr_size = sizeof(incoming_addr);
	int incoming_fd = accept(server.getServerSocket(), (struct sockaddr*) &incoming_addr, &incoming_addr_size);

	if(incoming_fd == -1) {
		printf("Accept error: %s\n", strerror(errno));
	}

	std::string message = "Hello World\n";

	send(incoming_fd, message.c_str(), message.length(), 0);

	char server_reply[6000];

	read(incoming_fd, server_reply, 6000);

	for(auto i : server_reply) {
		if(i == '\n') break;
		else std::cout << i;
	}

	return 0;
}