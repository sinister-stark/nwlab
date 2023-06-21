#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <dirent.h>

void serverRecv(int client_fd) {
	char str[100];
	char str1[1000];
	
	if(recv(client_fd, str, 100 * sizeof(char), 0) <= 0) {
		printf("Connection lost!\n");
		return;
	}
	
	if(!strcmp(str, "GET")) {
		DIR *d;
		struct dirent *dir;
		d = opendir(".");
		str1[0] = '\0';
		
		if (d) {
			while((dir = readdir(d)) != NULL) {
				if(strcmp(dir->d_name, ".") && strcmp(dir->d_name, "..") && strcmp(dir->d_name, "server.c") && strcmp(dir->d_name, "server")) {
					strcat(str1, dir->d_name);
					strcat(str1, "\n");
				}
			}
			closedir(d);
		}
		
		if(send(client_fd, str1, 1000 * sizeof(char), 0) <= 0) {
			printf("GET failed!\n");
			return;
		}
		
		if(send(client_fd, "200 OK", 7 * sizeof(char), 0) <= 0) {
			printf("GET failed!\n");
			return;
		} else {
			printf("GET successful!\n");
			return;
		}
	} if(!strcmp(str, "UPLOAD")) {
		if(recv(client_fd, str, 100 * sizeof(char), 0) <= 0) {
			printf("UPLOAD failed!\n");
			return;
		}
		
		if(recv(client_fd, str1, 1000 * sizeof(char), 0) <= 0) {
			printf("UPLOAD failed!\n");
			return;
		}
		
		FILE* fp = fopen(str, "w");
		fputs(str1, fp);
		fclose(fp);
		
		if(send(client_fd, "200 OK", 7 * sizeof(char), 0) <= 0) {
			printf("UPLOAD failed!\n");
			return;
		} else {
			printf("UPLOAD successful!\n");
			return;
		}
	} else if(!strcmp(str, "DOWNLOAD")) {
		if(recv(client_fd, str, 100 * sizeof(char), 0) <= 0) {
			printf("DOWNLOAD failed!\n");
			return;
		}
		
		FILE* fp = fopen(str, "r");
		
		if(fp == NULL) {
			str1[0] = '\0';
			
			if(send(client_fd, str1, sizeof(char), 0) <= 0) {
				printf("DOWNLOAD failed!\n");
				return;
			}
			
			if(send(client_fd, "400 BAD", 8 * sizeof(char), 0) <= 0) {
				printf("DOWNLOAD failed!\n");
				return;
			}
			
			return;
		}
		
		str1[0] = '\0';
		
		while(fgets(str, 100, fp) != NULL) {
			strcat(str1, str);
		}
		
		fclose(fp);
		
		if(send(client_fd, str1, 1000 * sizeof(char), 0) <= 0) {
			printf("DOWNLOAD failed!\n");
			return;
		}
		
		if(send(client_fd, "200 OK", 7 * sizeof(char), 0) <= 0) {
			printf("DOWNLOAD failed!\n");
			return;
		} else {
			printf("DOWNLOAD successful!\n");
			return;
		}
	} else if(!strcmp(str, "RENAME")) {
		if(recv(client_fd, str, 100 * sizeof(char), 0) <= 0) {
			printf("RENAME failed!\n");
			return;
		}
		
		if(recv(client_fd, str1, 100 * sizeof(char), 0) <= 0) {
			printf("RENAME failed!\n");
			return;
		}
		
		if(!rename(str, str1)) {
			if(send(client_fd, "200 OK", 7 * sizeof(char), 0) <= 0) {
				printf("RENAME failed!\n");
				return;
			}
		} else {
			if(send(client_fd, "400 BAD", 8 * sizeof(char), 0) <= 0) {
				printf("RENAME failed!\n");
				return;
			} else {
				printf("RENAME successful!\n");
				return;
			}
		}
	} else if(!strcmp(str, "DELETE")) {
		if(recv(client_fd, str, 100 * sizeof(char), 0) <= 0) {
			printf("DELETE failed!\n");
			return;
		}
		
		if(!remove(str)) {
			if(send(client_fd, "200 OK", 7 * sizeof(char), 0) <= 0) {
				printf("DELETE failed!\n");
				return;
			}
		} else {
			if(send(client_fd, "400 BAD", 8 * sizeof(char), 0) <= 0) {
				printf("DELETE failed!\n");
				return;
			} else {
				printf("DELETE successful!\n");
				return;
			}
		}
	} else {
		printf("Invalid request from client!\n");
	}
}

void main(int argc, char* argv[]) {
	int PORT;
	
	if(argc == 2) {
		PORT = atoi(argv[1]);
	} else {
		printf("Enter FTP server port!\n");
		exit(1);
	}
	
	int server_fd, client_fd;
	struct sockaddr_in address;
	int addrlen = sizeof(address);
	
	printf("FTP Server\n");
	
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
	
	if(listen(server_fd, 5) < 0) {
		printf("Listening failed!\n");
		exit(1);
	}
	
	while(1) {
		if((client_fd = accept(server_fd, (struct sockaddr*) &address, (socklen_t*) &addrlen)) < 0) {
			printf("Connection failed!\n");
			exit(1);
		} else {
			printf("Connected to client.\n");
		}
		
		serverRecv(client_fd);
		
		close(client_fd);
	}
	
	close(server_fd);
}
