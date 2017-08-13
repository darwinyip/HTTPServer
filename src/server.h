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

static std::map<std::string, std::string> content_type = {
    { ".html", "text/html" },
    { ".htm", "text/html" },
    { ".css", "text/css" },
    { ".gif", "image/gif" },
    { ".png", "image/png" },
    { ".jpg", "image/jpeg" },
    { ".json", "application/json"},
    { ".js", "application/javascript"},
    { ".xml", "application/xml" },
    { ".svg", "image/svg+xml" },
    { ".tif", "image/tiff" },
    { ".tiff", "image/tiff" },
    { ".ts", "application/typescript" },
    { ".zip", "application/zip" }
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
	std::string content_type;
	std::string full_response;
	std::string toString();
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