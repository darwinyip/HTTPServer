#ifndef HTTPSERVER_H
#define HTTPSERVER_H

#include <iostream>
#include <vector>

struct HttpHeader {
	std::string method;
	std::string uri;
	std::string version;
};

class HttpServer {
public:
	HttpServer(int port);
	~HttpServer();
	int getServerSocket();
	HttpHeader parseRequest(std::vector<char> request);

private:
	int server_socket_fd;
};

#endif