#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <ifaddrs.h>
#include <netdb.h>
#include <netinet/in.h>

#define bind_port 8080

int main(int argc, char *args[]) {

	int server;
	int client;
	struct sockaddr_in server_addr;
	struct sockaddr_in client_addr;
	socklen_t addr_len = sizeof(server_addr);

	FILE * file = fopen("./a.txt","w+");
	char buffer[1024] = {0,};
	size_t read_len;
	const char * response_packet = "HTTP/1.1 200 OK\r\nContent-Length: 5\r\nContent-Type: text/plain\r\n\r\nHello";

    //socket
	server = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (server < 0) {
		perror("socket() error");
		return 1;
	}
    /*
    int on = 1;
    //socket opt
	if (setsockopt(server, SOL_SOCKET, SO_REUSEADDR, (void*)&on, sizeof(on)) != 0) {
		perror("setsockopt() error");
		return 1;
	}
    */
    //socket bind
	server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    server_addr.sin_port = htons(bind_port);
	if (bind(server, (struct sockaddr*)&server_addr, addr_len) != 0) {
		perror("bind() error");
		return 1;
	}

	if (listen(server, 5) != 0) {
		perror("listen() error");
		return 1;
	}

	printf("listening... %d\n", bind_port);
	client = accept(server, (struct sockaddr*)&client_addr, &addr_len);
	if (client < 0) {
		perror("accept() error");
		return 1;
	}

	read_len = recv(client, buffer, sizeof(buffer), 0);
	fprintf(file,"%s", buffer);
	fclose(file);
	if (read_len > 0) {
		printf("RECV: %s", buffer);
	}

	send(client, response_packet, strlen(response_packet), 0);

	close(client);
	close(server);
	
    return 0;
}

