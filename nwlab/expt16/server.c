#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

void main() {
	int server_fd, client_fd;
	struct sockaddr_in address;
	int PORT, addrlen = sizeof(address);
	
	printf("Enter port: ");
	scanf("%d", &PORT);
	
	if((server_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		printf("Socket creation failed!\n");
		exit(1);
	}
	
	address.sin_family = AF_INET;
	address.sin_addr.s_addr = INADDR_ANY;
	address.sin_port = htons(PORT);
	
	if(bind(server_fd, (struct sockaddr*) &address, addrlen) < 0) {
		printf("Socket binding failed!\n");
		exit(1);
	}
	
	if(listen(server_fd, 1) < 0) {
		printf("Listening failed!\n");
		exit(1);
	}
	
	if((client_fd = accept(server_fd, (struct sockaddr*) &address, (socklen_t*) &addrlen)) < 0) {
		printf("Connection failed!\n");
		exit(1);
	}
	
	int n;
	
	while(1) {
		int k = recv(client_fd, &n, sizeof(int), 0);
		
		if(k < 0) {
			printf("Receive failed!\n");
			break;
		} else if(k == 0) {
			printf("Receive complete!\n");
			break;
		} else {
			printf("Received: %d\n", n);
		}
	}
	
	close(server_fd);
	close(client_fd);
}
