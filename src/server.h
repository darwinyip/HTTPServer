#ifndef SERVER_H
#define SERVER_H

#include <iostream>
#include <vector>
#include <map>

static std::map<int, std::string> status_code = {
    { 200, "OK" },
    { 403, "Forbidden"},
    { 404, "Not Found" },
    { 500, "Internal Server Error" }
};

struct RequestHeader {
	std::string method;
	std::string uri;
	std::string version;
};

struct Response {
	std::string version;
	int status_code;
	std::string reason_phrase;
	std::string full_response;
	std::string toString() {return version + " " + std::to_string(status_code) + " " + reason_phrase + "\n\n" + full_response + "\n";}
};

class HttpServer {
public:
	HttpServer(int port, std::string root);
	~HttpServer();
	void serve();
	RequestHeader parseRequest(std::string request);
	Response createResponse(std::string uri);
	int getServerSocket();

private:
	int server_socket_fd;
	std::string root;
};

#endif