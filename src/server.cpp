#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <cstring>
#include <iostream>
#include <unistd.h>
#include <errno.h>

int main(int argc, char const *argv[])
{
	int server_socket_fd = socket(AF_INET, SOCK_STREAM, 0);

	if(server_socket_fd == -1) {
		printf("Socket error: %s\n", strerror(errno));
	}

	struct sockaddr_in server_addr;

	std::memset(&server_addr, 0, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(8888);
	server_addr.sin_addr.s_addr = INADDR_ANY;

	if(bind(server_socket_fd, (struct sockaddr*) &server_addr, sizeof(server_addr)) == -1) {
		printf("Binding error: %s\n", strerror(errno));
	}

	if(listen(server_socket_fd, 10) == -1) {
		printf("Listen error: %s\n", strerror(errno));
	}

	struct sockaddr_storage incoming_addr;


	socklen_t incoming_addr_size = sizeof(incoming_addr);
	int incoming_fd = accept(server_socket_fd, (struct sockaddr*) &incoming_addr, &incoming_addr_size);

	if(incoming_fd == -1) {
		printf("Accept error: %s\n", strerror(errno));
	}

	send(incoming_fd, "const void *__buf", 17, 0);

	char server_reply[6000];

	read(incoming_fd, server_reply, 6000);

	for(auto i : server_reply) {
		std::cout << i;
	}

	return 0;
}