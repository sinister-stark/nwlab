#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>

int resolveClient(char* mail_id) {
	FILE* fp = fopen("dns", "r");
	
	if(fp == NULL)
		return -1;
	
	char str[120];
	int index = -1;
	
	while(fgets(str, 120, fp) != NULL) {
		if(strstr(str, mail_id) == str && str[strlen(mail_id)] == ':') {
			index = ftell(fp) - strlen(str);
		}
	}
	
	if(index == -1)
		return -1;
	else {
		fseek(fp, index, SEEK_SET);
		fgets(str, 120, fp);
		str[strlen(str) - 1] = '\0';
		
		return atoi(str + strlen(mail_id) + 1);
	}
}

void serverRecv(int client_fd) {
	char str[100];
	char mail[1000];
	int mda_port;
	
	while(1) {
		int k = recv(client_fd, str, 100 * sizeof(char), 0);
		
		if(k < 0) {
			printf("Receive failed!\n");
			break;
		} else if(k == 0 || !strcmp(str, "QUIT")) {
			printf("Connection closed.\n");
			break;
		} else if(!strcmp(str, "HELO")) {
			mail[0] = '\0';
			mda_port = -1;
		} else if(strstr(str, "MAIL FROM ") == str) {
			strcat(mail, str);
			strcat(mail, "\n");
			
			printf("Sender: %s\n", str + 10);
			
			str[0] = '\0';
			strcat(str, "200 OK");
			
			if(send(client_fd, str, (strlen(str) + 1) * sizeof(char), 0) < 0) {
				printf("Send failed!\n");
				break;
			}
		} else if(strstr(str, "RCP TO ") == str) {
			strcat(mail, str);
			strcat(mail, "\n");
			
			printf("Receiver: %s\n", str + 7);
			
			if((mda_port = resolveClient(str + 7)) == -1) {
				str[0] = '\0';
				strcat(str, "400 BAD");
			} else {
				str[0] = '\0';
				strcat(str, "200 OK");
			}
			
			if(send(client_fd, str, (strlen(str) + 1) * sizeof(char), 0) < 0) {
				printf("Send failed!\n");
				break;
			}
		} else if(strstr(str, "DATA ") == str) {
			strcat(mail, "DATA\n");
			strcat(mail, str + 5);
			strcat(mail, "\n");
			
			printf("Data:\n%s\n", str + 5);
			
			str[0] = '\0';
			strcat(str, "200 OK");
			
			if(send(client_fd, str, (strlen(str) + 1) * sizeof(char), 0) < 0) {
				printf("Send failed!\n");
				break;
			}
			
			struct sockaddr_in address;
			
			address.sin_family = AF_INET;
			address.sin_addr.s_addr = INADDR_ANY;
			address.sin_port = htons(mda_port);
			
			close(client_fd);
			
			if((client_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
				printf("Socket creation failed!\n");
				break;
			}
			
			if(connect(client_fd, (struct sockaddr*) &address, sizeof(address)) < 0) {
				printf("Connection failed!\n");
				break;
			}
			
			if(send(client_fd, mail, (strlen(mail) + 1) * sizeof(char), 0) < 0) {
				printf("Send failed!\n");
				break;
			} else {
				printf("Forwarded to MDA %d\n", mda_port);
			}
		} else {
			printf("Received invalid request from client: %s\n", str);
			break;
		}
	}
	
	close(client_fd);
}

void main() {
	int server_fd, client_fd, PORT;
	struct sockaddr_in address, cli_addr1, cli_addr2;
	int addrlen = sizeof(address);
	
	printf("SMTP MTA\n");
	
	printf("Enter port: ");
	scanf("%d", &PORT);
	
	if((server_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		printf("Socket creation failed!\n");
		exit(1);
	} else {
		printf("Server socket created.\n");
	}
	
	address.sin_family = AF_INET;
	address.sin_addr.s_addr = INADDR_ANY;
	address.sin_port = htons(PORT);
	
	if(bind(server_fd, (struct sockaddr*) &address, addrlen) < 0) {
		printf("Socket binding failed!\n");
		exit(1);
	}
	
	if(listen(server_fd, 5) < 0) {
		printf("Listening failed!\n");
		exit(1);
	}
	
	while(1) {
		if((client_fd = accept(server_fd, (struct sockaddr*) &address, (socklen_t*) &addrlen)) < 0) {
			printf("Connection failed!\n");
			return;
		}
		
		printf("Connected to client.\n");
		
		serverRecv(client_fd);
	}
	
	close(server_fd);
}
