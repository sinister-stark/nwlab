#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>

void closeConn(int client_fd) {
	char* str = "QUIT";
	printf("Connection closed.\n");
	send(client_fd, str, (strlen(str) + 1) * sizeof(char), 0);
	close(client_fd);
}

void clientSend(struct sockaddr_in serv_addr, char* mail_id) {
	char str[100];
	char str1[1000];
	int client_fd;
	
	printf("Enter \"HELO\" to initiate mail transfer: ");
	scanf("%s", str);
	getchar();
	
	if(!strcmp(str, "HELO")) {
		if((client_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
			printf("Socket creation failed!\n");
			return;
		}
		
		if(connect(client_fd, (struct sockaddr*) &serv_addr, sizeof(serv_addr)) < 0) {
			printf("Connection failed!\n");
			close(client_fd);
			return;
		}
		
		if(send(client_fd, str, 100 * sizeof(char), 0) < 0) {
			printf("Send failed!\n");
			closeConn(client_fd);
			return;
		}
	} else {
		printf("Invalid input!\n");
		return;
	}
	
	if(strlen(str) != 0) {
		str1[0] = '\0';
		strcat(str1, "MAIL FROM ");
		strcat(str1, mail_id);
		
		if(send(client_fd, str1, (strlen(str1) + 1) * sizeof(char), 0) < 0) {
			printf("Send failed!\n");
			closeConn(client_fd);
			return;
		} else {
			if (recv(client_fd, str, 100 * sizeof(char), 0) < 0) {
				printf("Receive failed!\n");
				closeConn(client_fd);
				return;
			} else {
				if(strcmp(str, "200 OK") != 0) {
					printf("Bad response!\n");
					closeConn(client_fd);
					return;
				}
			}
		}
	} else {
		printf("Invalid response!\n");
		closeConn(client_fd);
		return;
	}
	
	printf("MAIL TO: ");
	fgets(str, 100, stdin);
	str[strlen(str) - 1] = '\0';
	
	if(strlen(str) != 0) {
		str1[0] = '\0';
		strcat(str1, "RCP TO ");
		strcat(str1, str);
		
		if(send(client_fd, str1, (strlen(str1) + 1) * sizeof(char), 0) < 0) {
			printf("Send failed!\n");
			closeConn(client_fd);
			return;
		} else {
			if (recv(client_fd, str, 100 * sizeof(char), 0) < 0) {
				printf("Receive failed!\n");
				closeConn(client_fd);
				return;
			} else {
				if(strcmp(str, "200 OK") != 0) {
					printf("Bad response!\n");
					closeConn(client_fd);
					return;
				}
			}
		}
	} else {
		printf("Invalid response!\n");
		closeConn(client_fd);
		return;
	}
	
	str1[0] = '\0';
	strcat(str1, "DATA ");
	printf("DATA:\n");
	
	while(1) {
		fgets(str, 100, stdin);
		
		if(!strcmp(str, ".\n")) {
			str1[strlen(str1) - 1] = '\0';
			
			if(send(client_fd, str1, (strlen(str1) + 1) * sizeof(char), 0) < 0) {
				printf("Send failed!\n");
				closeConn(client_fd);
				return;
			} else {
				if (recv(client_fd, str, 100 * sizeof(char), 0) < 0) {
					printf("Receive failed!\n");
					closeConn(client_fd);
					return;
				} else {
					if(!strcmp(str, "200 OK")) {
						printf("Mail successfully sent.\n");
						return;
					} else { 
						printf("Bad response!\n");
						closeConn(client_fd);
						return;
					}
				}
			}
			
			break;
		} else {
			strcat(str1, str);
		}
	}
	
	closeConn(client_fd);
}

void clientRecv(struct sockaddr_in serv_addr, char* mail_id) {
	char str[1000];
	int client_fd;
	
	if((client_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		printf("Socket creation failed!\n");
		return;
	}
	
	if(connect(client_fd,(struct sockaddr*) &serv_addr, sizeof(serv_addr)) < 0) {
		printf("Connection failed!\n");
		close(client_fd);
		return;
	}
	
	strcpy(str, "RECV MAIL ");
	strcat(str, mail_id);
	
	if(send(client_fd, str, 1000 * sizeof(char), 0) < 0) {
		printf("Send failed!\n");
		return;
	}
	
	while(1) {
		int k = recv(client_fd, str, 1000 * sizeof(char), 0);
		
		if(!strcmp(str, "200 OK"))
			break;
		else if(k < 0) {
			printf("Receive failed!\n");
			break;
		} else {
			printf("\nReceived mail!\n\n%s", str);
		}
	}
	
	close(client_fd);
}

void registerClient(char* str, int port) {
	FILE* fp = fopen("dns", "a");
	
	strcat(str, ":");
	
	char portStr[10];
	sprintf(portStr, "%d", port);
	
	strcat(str, portStr);
	strcat(str, "\n");
	
	fputs(str, fp);
	
	printf("Successfully registered mail id!\n");
	
	fclose(fp);
}

void main(int argc, char *argv[]) {
	int port1, port2;
	struct sockaddr_in serv_addr;
	
	if(argc == 3) {
		port1 = atoi(argv[1]);
		port2 = atoi(argv[2]);
	}
	else {
		printf("Enter SMTP MTA and MDA ports!\n");
		exit(1);
	}
	
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = INADDR_ANY;
	
	printf("SMPT Mailing System\n\n");
	
	char mail_id[100];
	printf("Enter your mail id: ");
	scanf("%s", mail_id);
	
	registerClient(strdup(mail_id), port2);
	
	int n;
	
	while(1) {
		printf("\n1. Send mail\n2. Receive mail\nEnter your command: ");
		scanf("%d", &n);
		
		if(n == 1) {
			serv_addr.sin_port = htons(port1);
			clientSend(serv_addr, mail_id);
		} else if(n == 2) {
			serv_addr.sin_port = htons(port2);
			clientRecv(serv_addr, mail_id);
		} else {
			printf("Invalid command!\n");
		}
	}
}
