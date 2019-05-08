#include<sys/types.h>
#include<sys/socket.h>
#include<stdio.h>
#include<stdlib.h>
#include<errno.h>
#include<string.h>
#include<netinet/in.h>
#include<unistd.h>
#include<netinet/tcp.h>
#include<sys/time.h>
#include<sys/ioctl.h>
#include<assert.h>

struct sockaddr_in server_sockaddr, client_sockaddr;
int sock_fd;
int client_fd;
char buffer_get[1024];
char buffer_send[1024];
int send_bytes;
int get_bytes;
fd_set inset;
int* size;
int main()
{
    if ((sock_fd = socket(AF_INET,SOCK_STREAM,0)) == -1) { 
	perror("socket");
	exit(1);
    }
    server_sockaddr.sin_family = AF_INET;
    server_sockaddr.sin_port = htons(4300);
    server_sockaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    bzero(&server_sockaddr.sin_zero, 8);
    int i = 1;
    setsockopt(sock_fd, SOL_SOCKET, SO_REUSEADDR, &i, sizeof(i));
    if (bind(sock_fd, (struct sockaddr*)&server_sockaddr, sizeof(struct sockaddr)) == -1) {
	perror("bind");
	exit(1);
    }
    printf("------------------------bind---------------------------------\n");
    if (listen(sock_fd, 5) == -1) {
	perror("listen");
	exit(1);
    }
    printf("------------------------listen-------------------------------\n");
    FD_ZERO(&inset);
    FD_SET(sock_fd, &inset);
    while (1) {
	size = malloc(sizeof(int)*1000);
	memset(buffer_get, 0, sizeof(buffer_get));
	memset(buffer_send, 0, sizeof(buffer_send));
	struct timeval timeout={100,0}; 
	int h = select(FD_SETSIZE, &inset, (fd_set *)0, (fd_set *)0, &timeout);
	switch(h) {
	    case -1:
		exit(-1);
		break;
	    case 0:
		break; 
	    default:
		for (int fd = 0; fd < FD_SETSIZE; fd ++) {
		    if (FD_ISSET(fd, &inset) > 0) {
			if (fd == sock_fd) {
			    if ((client_fd = accept(sock_fd, (struct sockaddr*)&client_sockaddr, (socklen_t*)&size)) == -1) {
     		    		perror("accept");
	    	    		exit(1);
	            	    } 
	            	    printf("-----------------------accept-------------------------------\n");
			    FD_SET(client_fd, &inset);
			} else {
			    while(1) {
				get_bytes = read(fd, buffer_get, sizeof(buffer_get));
			    	if (get_bytes < 0) {
				    perror("read");
				    exit(1);
			        } else if (get_bytes == 0) {
			     	    close(fd);
				    FD_CLR(fd, &inset);
				    printf("-----------------------close-------------------------------\n");\
				    break;
			        } else { 
				    printf("%s\n", buffer_get);
				    sprintf(buffer_send, "%s", "server-");
				    sprintf(buffer_send+7, "%s", buffer_get);
				    printf("%s\n", buffer_send);
				    send_bytes = write(fd, buffer_send, sizeof(buffer_send));
				    if(send_bytes < 0) {
					perror("write");
					exit(1);
				    }
				    printf("-----------------------write-------------------------------\n");
			        }
			    }
			}
		    }
		}
		break;
	    
	}
	free(size);
    }
    close(sock_fd);
    FD_ZERO(&inset);
    return 0;
}		

