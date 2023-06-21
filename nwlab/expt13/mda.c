#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>

int m_len = 0;
char mails[50][1000];

void serverRecv(int client_fd) {
	char str[1000];
	
	while(1) {
		int k = recv(client_fd, str, 1000 * sizeof(char), 0);
		
		if(k < 0) {
			printf("Receive failed!\n");
			return;
		} else if(k == 0) {
			printf("Connection closed.\n");
			return;
		} else if(strstr(str, "MAIL FROM ") == str) {
			strcpy(mails[m_len], str);
			m_len++;
			
			printf("Mail received and queued!\n");
			break;
		} else if(strstr(str, "RECV MAIL ") == str) {
			char* mail_id = strdup(str + 10);
			
			for(int i = 0; i < m_len; i++) {
				char* recp = strstr(mails[i], "RCP TO ");
				recp = strdup(recp + 7);
				
				recp = strtok(recp, "\n");
				
				if(!strcmp(mail_id, recp)) {
					if(send(client_fd, mails[i], (strlen(mails[i]) + 1) * sizeof(char), 0) < 0) {
						printf("Send failed!\n");
						break;
					}
				} else {
					printf("Recp: %s", recp);
				}
			}
			
			char* str1 = "200 OK";
			
			if(send(client_fd, str1, (strlen(str1) + 1) * sizeof(char), 0) < 0) {
				printf("Send failed!\n");
				break;
			} else {
				printf("Queued mails sent to %s.\n", mail_id);
				break;
			}
		} else {
			printf("Invalid request!\n");
			break;
		}
	}
	
	close(client_fd);
}

void main() {
	int server_fd, client_fd, PORT;
	struct sockaddr_in address, cli_addr1, cli_addr2;
	int addrlen = sizeof(address);
	
	printf("SMTP MDA\n");
	
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
			continue;
		}
		
		printf("Connected to client.\n");
		
		serverRecv(client_fd);
	}
	
	close(server_fd);
}
