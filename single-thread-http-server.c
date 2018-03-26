#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

//socket
#include <arpa/inet.h>
#include <stdbool.h>
#include "http-parser.h"

struct sock_client
{
	int client_sock;
	struct sockaddr_in * client_addr;
	socklen_t client_addr_len;
};
void s_handle_client(int client_sock, struct sockaddr_in * client_addr, socklen_t client_addr_len);


int main(int argc, char *args[]) {

    //-----------------------------------------------------------------------
    // Server
    //-----------------------------------------------------------------------
	int server;
	int client;
	struct sockaddr_in server_addr;
	struct sockaddr_in client_addr;
	socklen_t addr_len = sizeof(server_addr);
	int bind_port = 8089;
	int on = 1;

	//pthread
	pthread_t pthread[100];
	int pthread_i = 0;

	//socket 생성
	server = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (server < 0) {
		perror("socket() error");
		return 1;
	}

	//socket 옵션

	if (setsockopt(server, SOL_SOCKET, SO_REUSEADDR, (void*)&on, sizeof(on)) != 0) {

		perror("setsockopt() error");
		return 1;
	}

	//socket binding
	memset(&server_addr, 0, sizeof(struct sockaddr_in));
	server_addr.sin_family = AF_INET;						//ipv4	 인터넷 프로토콜
	server_addr.sin_addr.s_addr = htonl(INADDR_ANY);		//32bit ipv4 주소
	server_addr.sin_port = htons(bind_port);				//port

	if (bind(server, (struct sockaddr*)&server_addr, addr_len) != 0) {
		perror("bind() error");
		return 1;
	}

	//listen
	if (listen(server, 5) != 0) {
		perror("listen() error");
		return 1;
	}
	printf("listening... %d\n", bind_port);

	//accept
	//while (1) {
		client = accept(server, (struct sockaddr*)&client_addr, &addr_len);

		if (client < 0) {
			perror("accept() error");
            //fprintf(log,"accept() error\n");
			return 1;
		}

        s_handle_client(client, &client_addr, addr_len);

	//}

	close(server);
    //-----------------------------------------------------------------------

    return 0;
}


void s_handle_client(int client_sock, struct sockaddr_in * client_addr, socklen_t client_addr_len) {
	int read_len = -1;
	char header_buffer[2048] = {0,};
	size_t p = 0;
	http_request_line_t request = {0,};
	http_header_fields_t fields = {0,};
	char response_header[4096] = {0,};
	char response_content[4096] = {0,};
	char client_ip_address[128] = {0,};
    char path[256] = "/Users/sangjin/backup/simplehttpserver";

	//recv의 내용크기가 내가 할당한 buffer 크기보다 크지 않으면서 &&
	//마지막 \r\n\r\n이전내용까지 header_buffer에 넣기
	while (p < sizeof(header_buffer) && (read_len = recv(client_sock, header_buffer + p, 1, 0)) > 0) {
		if (p > 4 &&
			header_buffer[p - 3] == '\r' && header_buffer[p - 2] == '\n' &&
			header_buffer[p - 1] == '\r' && header_buffer[p] == '\n') {
			break;
		}
		p++;
		read_len = -1;
	}

	if (read_len < 0) {
		printf("socket closed\n");
		return;
	}

	//클라이언트의 인터넷 프로토콜, 2진수로 된 ip, 10진수로 변환받을 ip buffer, buffer size
	// 2진수의 ip address -> 10진수의 ip address
	inet_ntop(client_addr->sin_family, &client_addr->sin_addr, client_ip_address, sizeof(client_ip_address));


	parseHttpHeader(parseHttpRequestLine(header_buffer, &request), &fields);
    char uri_buf[256] = {0};
    char reference_buf[256] = {0};
    int reference_index = 0;
    int i;
    for(i =0 ;i < strlen(request.uri); i ++)
    {
        if(request.uri[i] == '?')
        {
            break;
        }
        else
        {
            uri_buf[i] = request.uri[i];
        }
    }
    for(++i ;i < strlen(request.uri); i ++)
    {
        reference_buf[reference_index] = request.uri[i];
        reference_index++;
    }
    memset(request.uri, 0 , strlen(request.uri));
    strcpy(request.uri, uri_buf);
    write(1, request.uri, strlen(request.uri));
    write(1, reference_buf, strlen(reference_buf));
    printf("%s\n", request.uri);
    printf("%s\n", reference_buf);
/*
    char ** refer_infor = NULL;
    if(!isspace(reference_buf))
    {
        *refer_infor = malloc(sizeof(char*)*64);
        for(int j = 0 ; j < 2 ;j++)
        {
            refer_infor[j] = malloc(sizeof(char)*64);
        }
        int p = 0, q = 0;
        int t = 0, s = 0;
        char key[256] = {0}, value[256] = {0};
        bool iskey = true;
        for(int j = 0; j < strlen(reference_buf) ; j ++)
        {
            if(reference_buf[j] == '=')
            {
                iskey = false;
            }
            else if(reference_buf[j] == '&')
            {
                strcmp(refer_infor[t][s], key);

            }

            if(iskey)
            {
                key[p] = reference_buf[j];
                p++;
            }
            else
            {
                value[q] = reference_buf[j];
                q++;
            }
        }
    }*/
	if(strcmp(request.uri, "/") == 0) {
        //fprintf(content, "%s\n", request.uri);
        snprintf(response_content, sizeof(response_content), "<h1>Your Request Information</h1>"
                "<p>You IP Address: %s:%d</p>"
                "<ul>"
                "<li>Method: %s</li>"
                "<li>Path: %s</li>"
                "<li>Protocol: %s</li>"
                "</ul>", client_ip_address, ntohs(client_addr->sin_port), request.method, request.uri,
                 request.protocol);
        //fprintf(content, "%s\n", request.uri);

    }
    /*else{
        strcat(path,request.uri);
        execl(path, client_sock, path, NULL);
        FILE *file = fopen(path, "r")
        fgets(response_content, sizeof(response_content),file);
        fclose(file);
    }*/
	else if(strcmp(request.uri, "/test/bye") == 0)
    {
        //write(1,"bye\n", 4);
        //write(1,request.uri, strlen(request.uri));
        char pwd[] = "/Users/sangjin/backup/simplehttpserver/test/bye";
        //execl(pwd, client_sock, path, NULL);
        FILE *file = fopen(pwd, "r");
		fgets(response_content, sizeof(response_content),file);
		fclose(file);

	}
    else if(strcmp(request.uri, "/test/hello") == 0)
    {
        //write(1,"hello", 5);
        char pwd[] = "/Users/sangjin/backup/simplehttpserver/test/hello";
        char sock_buf[2] ;
        sprintf(sock_buf,"%d", client_sock);
        char *refer[] = {"hello",sock_buf,"/Users/sangjin/backup/simplehttpserver/test/hello", NULL};
        printf("%s\n", refer[0]);printf("%s\n", refer[1]);

        execv("/Users/sangjin/CLionProjects/hello/cmake-build-debug", refer);
//        execl("/Users/sangjin/CLionProjects/hello/cmake-build-debug/hello" , NULL);
        FILE *file = fopen(pwd, "r");
        fgets(response_content, sizeof(response_content),file);
        fclose(file);

    }
    else if(strcmp(request.uri, "/test/hey") == 0)
    {
        //write(1,"hello", 5);
        char pwd[] = "/Users/sangjin/backup/simplehttpserver/test/hey";
        //execl(pwd, client_sock, path, NULL);
        FILE *file = fopen(pwd, "r");
        fgets(response_content, sizeof(response_content),file);
        fclose(file);

    }
    strcat(response_content, reference_buf);
	snprintf(response_header, sizeof(response_header), "HTTP/1.1 200 OK\r\n"
			"Content-Type: text/html\r\n"
			"Content-Length: %ld\r\n"
			"\r\n", strlen(response_content));


    send(client_sock, response_header, strlen(response_header), 0);
    send(client_sock, response_content, strlen(response_content), 0);

    releaseHeaderFields(&fields);
    close(client_sock);
}
