#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>

void get(int client_fd) {
	char filename[100];
	char str[100];
	char str1[1000];
	printf("Enter filename: ");
	scanf("%s", filename);
	
	if(send(client_fd, filename, 100 * sizeof(char), 0) <= 0) {
		printf("GET failed!\n");
		return;
	} else {
		if(recv(client_fd, str1, 1000 * sizeof(char), 0) <= 0) {
			printf("GET failed!\n");
			return;
		}
		
		int pid;
		
		if(recv(client_fd, &pid, sizeof(int), 0) <= 0) {
			printf("GET failed!\n");
			return;
		}
		
		if(recv(client_fd, str, 100 * sizeof(char), 0) <= 0) {
			printf("GET failed!\n");
			return;
		}
		
		if(!strcmp(str, "200 OK")) {
			FILE* fp = fopen(filename, "w");
			fputs(str1, fp);
			
			printf("GET successful (PID %d)!\n", pid);
		
			fclose(fp);
		} else {
			printf("GET failed (PID %d)!\n", pid);
		}
	}
}

void main() {
	int client_fd, PORT;
	struct sockaddr_in serv_addr;
	
	printf("File Client\n");
	
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = INADDR_ANY;
	
	while(1) {
		printf("Enter file server port: ");
		scanf("%d", &PORT);
		
		serv_addr.sin_port = htons(PORT);
		
		client_fd = socket(AF_INET, SOCK_STREAM, 0);
		
		if(client_fd < 0) {
			printf("Socket creation failed!\n");
			exit(1);
		}
		
		if(connect(client_fd, (struct sockaddr*) &serv_addr, sizeof(serv_addr)) < 0) {
			printf("Connection failed!\n");
			exit(1);
		}
		
		get(client_fd);
		
		close(client_fd);
	}
}
