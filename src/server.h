#ifndef SERVER_H
#define SERVER_H

#include <iostream>
#include <vector>

struct HttpHeader {
	std::string method;
	std::string uri;
	std::string version;
};

class HttpServer {
public:
	HttpServer(int port, std::string root);
	~HttpServer();
	int getServerSocket();
	HttpHeader parseRequest(std::string request);
	std::string createResponse(std::string uri);

private:
	int server_socket_fd;
	std::string root;
};

#endif