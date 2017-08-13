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
#include <fstream>
#include "server.h"

HttpServer::HttpServer(int port, std::string root) : root(root) {
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

	int option = 1;
	if(setsockopt(this->server_socket_fd, SOL_SOCKET, SO_REUSEADDR, &option, sizeof(option)) == -1) {
		throw std::runtime_error(std::string("setsockopt error: ") + strerror(errno));
	}

	if(bind(this->server_socket_fd, res->ai_addr, res->ai_addrlen) == -1) {
		throw std::runtime_error(std::string("Binding error: ") + strerror(errno));
	}

	if(listen(this->server_socket_fd, 10) == -1) {
		throw std::runtime_error(std::string("Listening error: ") + strerror(errno));
	}
}

HttpServer::~HttpServer() {
	shutdown(this->server_socket_fd, SHUT_RDWR);
	close(this->server_socket_fd);
}

int HttpServer::getServerSocket() {
	return this->server_socket_fd;
}

void HttpServer::serve() {
	while(true) {
		struct sockaddr_storage incoming_addr;
		socklen_t incoming_addr_size = sizeof(incoming_addr);
		int incoming_fd = accept(this->server_socket_fd, (struct sockaddr*) &incoming_addr, &incoming_addr_size);
		if(incoming_fd == -1) {
			throw std::runtime_error(std::string("Accept error: ") + strerror(errno));
		}

		std::vector<char> buffer(4096);
		read(incoming_fd, buffer.data(), buffer.size());
		std::string request(buffer.begin(), buffer.end());

		RequestHeader header = parseRequest(request);
		if(header.method == "GET") {
			std::string response = createResponse(header.uri).toString();
			send(incoming_fd, response.c_str(), response.length(), 0);
		}
		close(incoming_fd);
	}
}

RequestHeader HttpServer::parseRequest(std::string request) {
	RequestHeader header;
	std::string line;
	std::istringstream iis(request);

	while(std::getline(iis, line)) {
		if(line.size() > 1) { // Account for newline character
			std::istringstream iss(line);
			std::vector<std::string> header_field{std::istream_iterator<std::string>{iss}, std::istream_iterator<std::string>{}};

			if(header_field[0] == "GET") {
				header.method = header_field[0];
				header.uri = header_field[1];
				header.version = header_field[2];
			}
		}
	}
	return header;
}

Response HttpServer::createResponse(std::string uri) {
	std::ifstream file;
	file.exceptions (std::ifstream::failbit | std::ifstream::badbit);
	Response response;
	response.version = "HTTP/1.0";

	try {
		file.open(this->root+uri, std::ios::out);
		std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
		file.close();

		response.status_code = 200;
		response.reason_phrase = status_code[200];
		response.content_type = "Content-type: " + content_type[uri.substr(uri.find_last_of("."))];
		response.full_response = content;
		return response;
	}
	catch(std::ifstream::failure e) {
		if(errno == ENOENT) {
			response.status_code = 404;
			response.reason_phrase = status_code[404];
			response.full_response = "Sorry Not Found";
			return response;
		}
		else {
			response.status_code = 500;
			response.reason_phrase = status_code[500];
			response.full_response = "";
			return response;
		}
	}
}

std::string Response::toString() {
	return version + " " + std::to_string(status_code) + " " + reason_phrase + "\n" + content_type + "\n\n" +	full_response + "\n";
}

