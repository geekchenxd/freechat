/*********************************************************
 * Filename    : client.c
 * Description : a tcp socket comminute demo
 * Author      : ChenXiaodong --Linux development engineer
 * E-mail      : www.chenxd@hzhytech.com
 * Created Time: 5th JUN,2017
 *********************************************************/
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <arpa/inet.h>
#include <sys/un.h>
#include <pthread.h>

int global_sock = -1;

#define LISTEN_BACKLOG 8
#ifdef AF_UNIX_WAY
#define MY_SOCK_PATH "/somepath"
#endif
#define handle_error(msg) \
	do {perror(msg); exit(EXIT_FAILURE);} while(0)

void *recv_call(void *arg)
{
	ssize_t recv_len = 0;
	char buff[1024] = {0x0};

	while (1) {
		recv_len = read(global_sock, buff, sizeof(buff));
		if (recv_len > 0) {
			printf("%s", buff);
			memset(&buff[0], 0x0, sizeof(buff));
		} 
	}
	return NULL;
}

int main(int argc, char *argv[])
{
	int sockfd;
	int PORT;
	pthread_t recv_id;
	char *ADDR = NULL;
	if (argc < 3) {
		printf("Usage:%s [server ip] [port]\n", argv[0]);
		exit(EXIT_SUCCESS);
	}
	ADDR = argv[1];
	PORT = atoi(argv[2]);
#ifdef AF_UNIX_WAY
	struct sockaddr_un serv_addr;
#else
	struct sockaddr_in serv_addr;
#endif
	char data[1024] = {0x0};
	size_t sendlen = 0;

	/* 
	 * 1. Create a socket 
	 */
#ifdef AF_UNIX_WAY
	sockfd = socket(AF_UNIX, SOCK_STREAM, 0);
#else
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
#endif
	if (sockfd == -1) {
		handle_error("socket create");
	}

	/*
	 * 2. bind address
	 */
#ifdef AF_UNIX_WAY
	/* Clear structure */
	memset(&serv_addr, 0, sizeof(struct sockaddr_un));
	serv_addr.sun_family = AF_UNIX;
	strncpy(serv_addr.sun_path, MY_SOCK_PATH, sizeof(serv_addr.sun_path) - 1);
#else
	/* Clear structure */
	memset(&serv_addr, 0, sizeof(struct sockaddr_in));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(PORT);
	serv_addr.sin_addr.s_addr = inet_addr(ADDR);
#endif
	global_sock = sockfd;

	/*
	 * 3. request for connect to server
	 */
	if (connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(struct sockaddr)) == -1)
		handle_error("connect");
	printf("connect to server OK\n");

	if (pthread_create(&recv_id, NULL, recv_call, NULL)) {
		perror("pthread create");
		exit(1);
	}
	/*
	 * 4.  read from stdin and send to server
	 */
	while(fgets(data, sizeof(data), stdin)) {
		if (strncmp(data, "exit", 4) == 0)
			break;
		//sendlen = send(sockfd, (void *)data, sizeof(data), 0);
		//sendlen = write(sockfd, (void *)data, strlen(data));
		sendlen = sendto(sockfd, (void *)data, sizeof(data), 0, NULL, 0);
		if (sendlen < 0) 
			handle_error("send");
	}

	shutdown(sockfd, SHUT_RDWR);
	return 0;
}
