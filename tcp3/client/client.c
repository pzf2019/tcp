#include<sys/types.h>
#include<sys/socket.h>
#include<stdio.h>
#include<stdlib.h>
#include<errno.h>
#include<string.h>
#include<unistd.h>
#include<netinet/in.h>
#include <netdb.h>
#include<arpa/inet.h>
int main(int argc,char* argv[])
{

	struct sockaddr_in client_addr;
	int sockfd;
	
	if(argc < 3) {
		perror("argc");
		exit(1);
	}
	if((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
		perror("socket");
		exit(1);
	}
	
	const char* ichp = argv[1];
	int port = atoi(argv[2]);

	client_addr.sin_family = AF_INET;
	client_addr.sin_port = htons(port);
	inet_pton(AF_INET, ichp, &client_addr.sin_addr);
	bzero(&(client_addr.sin_zero), 8);
	if(connect(sockfd, (struct sockaddr *)&client_addr, sizeof(struct sockaddr)) == -1) {
		perror("connect");
		exit(1); 
	}
	
	int send_bytes;
	int get_bytes;
	char buffer_send[1024];
	char buffer_get[1024];
	//memset(buffer_send, 0, sizeof(buffer_send));
	sprintf(buffer_send, "%s", argv[3]);
	//printf(buffer_send);
	for(int i = 0; i < 100; i ++) {
		if((send_bytes = send(sockfd, buffer_send, strlen(buffer_send), 0)) == -1) {
			perror("send");
			exit(1);
		}
		printf("%s\n", buffer_send);
		if((get_bytes = recv(sockfd, buffer_get, 1024, 0)) == -1){
			perror("recv");
			exit(1);
		}
		printf("%s\n", buffer_get);
		sleep(2);
	}

	
	close(sockfd);
	printf("close ready\n");
	exit(0);
}
	
	


