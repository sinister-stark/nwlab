#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>

void ftp_get(int client_fd) {
	char str[8];
	char str1[1000];
	
	if(send(client_fd, "GET", 4 * sizeof(char), 0) <= 0) {
		printf("GET failed!\n");
		return;
	}
	
	if(recv(client_fd, str1, 1000 * sizeof(char), 0) <= 0) {
		printf("GET failed!\n");
		return;
	} else {
		if(recv(client_fd, str, 8 * sizeof(char), 0) <= 0) {
			printf("GET failed!\n");
			return;
		}
		
		if(!strcmp(str, "200 OK")) {
			printf("GET successful!\nFiles in FTP server:\n%s", str1);
		} else {
			printf("GET failed!\n");
		}
	}
}

void ftp_upload(int client_fd) {
	if(send(client_fd, "UPLOAD", 7 * sizeof(char), 0) <= 0) {
		printf("UPLOAD failed!\n");
		return;
	}
	
	char filename[100];
	char str[100];
	char str1[1000];
	
	printf("Enter filename: ");
	scanf("%s", filename);
	
	FILE* fp = fopen(filename, "r");
	
	if(fp == NULL) {
		printf("File doesn't exist!\n");
		return;
	}
	
	if(send(client_fd, filename, 100 * sizeof(char), 0) <= 0) {
		printf("Send failed!\n");
		fclose(fp);
		return;
	} else {
		str1[0] = '\0';
		
		while(fgets(str, 100, fp) != NULL) {
			strcat(str1, str);
		}
		
		if(send(client_fd, str1, 1000 * sizeof(char), 0) <= 0) {
			printf("UPLOAD failed!\n");
			fclose(fp);
			return;
		}
		
		if(recv(client_fd, str, 7 * sizeof(char), 0) <= 0) {
			printf("UPLOAD failed!\n");
			fclose(fp);
			return;
		}
		
		if(!strcmp(str, "200 OK")) {
			printf("UPLOAD successful!\n");
		}
		
		fclose(fp);
	}
}

void ftp_download(int client_fd) {
	if(send(client_fd, "DOWNLOAD", 9 * sizeof(char), 0) <= 0) {
		printf("Send failed!\n");
		return;
	}
	
	char filename[100];
	char str[100];
	char str1[1000];
	printf("Enter filename: ");
	scanf("%s", filename);
	
	if(send(client_fd, filename, 100 * sizeof(char), 0) <= 0) {
		printf("DOWNLOAD failed!\n");
		return;
	} else {
		if(recv(client_fd, str1, 1000 * sizeof(char), 0) <= 0) {
			printf("DOWNLOAD failed!\n");
			return;
		}
		
		if(recv(client_fd, str, 100 * sizeof(char), 0) <= 0) {
			printf("DOWNLOAD failed!\n");
			return;
		}
		
		if(!strcmp(str, "200 OK")) {
			FILE* fp = fopen(filename, "w");
			fputs(str1, fp);
			
			printf("DOWNLOAD successful!\n");
		
			fclose(fp);
		} else {
			printf("DOWNLOAD failed!\n");
		}
	}
}

void ftp_rename(int client_fd) {
	if(send(client_fd, "RENAME", 7 * sizeof(char), 0) <= 0) {
		printf("RENAME failed!\n");
		return;
	}
	
	char filename[100];
	char filename1[100];
	char str[8];
	
	printf("Enter old filename: ");
	scanf("%s", filename);
	
	printf("Enter new filename: ");
	scanf("%s", filename1);
	
	if(send(client_fd, filename, 100 * sizeof(char), 0) <= 0) {
		printf("RENAME failed!\n");
		return;
	} else {
		if(send(client_fd, filename1, 100 * sizeof(char), 0) <= 0) {
			printf("RENAME failed!\n");
			return;
		} else {
			if(recv(client_fd, str, 8 * sizeof(char), 0) <= 0) {
				printf("RENAME failed!\n");
				return;
			}
			
			if(!strcmp(str, "200 OK")) {
				printf("RENAME successful!\n");
			} else {
				printf("RENAME failed!\n");
			}
		}
	}
}

void ftp_delete(int client_fd) {
	if(send(client_fd, "DELETE", 7 * sizeof(char), 0) <= 0) {
		printf("DELETE failed!\n");
		return;
	}
	
	char filename[100];
	char str[8];
	printf("Enter filename: ");
	scanf("%s", filename);
	
	if(send(client_fd, filename, 100 * sizeof(char), 0) <= 0) {
		printf("DELETE failed!\n");
		return;
	} else {
		if(recv(client_fd, str, 8 * sizeof(char), 0) <= 0) {
			printf("DELETE failed!\n");
			return;
		}
		
		if(!strcmp(str, "200 OK")) {
			printf("DELETE successful!\n");
		} else {
			printf("DELETE failed!\n");
		}
	}
}

void main() {
	int client_fd, PORT;
	struct sockaddr_in serv_addr;
	
	printf("FTP Client\n");
	
	printf("Enter FTP server port: ");
	scanf("%d", &PORT);
	
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = INADDR_ANY;
	serv_addr.sin_port = htons(PORT);
	
	while(1) {
		client_fd = socket(AF_INET, SOCK_STREAM, 0);
		
		if(client_fd < 0) {
			printf("Socket creation failed!\n");
			exit(1);
		}
		
		if(connect(client_fd, (struct sockaddr*) &serv_addr, sizeof(serv_addr)) < 0) {
			printf("Connection failed!\n");
			exit(1);
		}
		
		int option = 0;
		printf("\n1. GET\n2. UPLOAD\n3. DOWNLOAD\n4. RENAME\n5. DELETE\n6. QUIT\nEnter your command: ");
		scanf("%d", &option);
		getchar();
		
		switch(option) {
			case 1:
				ftp_get(client_fd);
				break;
			case 2:
				ftp_upload(client_fd);
				break;
			case 3:
				ftp_download(client_fd);
				break;
			case 4:
				ftp_rename(client_fd);
				break;
			case 5:
				ftp_delete(client_fd);
				break;
			case 6:
				printf("Exit.\n");
				exit(0);
			default:
				printf("Invalid command!\n");
				break;
		}
		
		close(client_fd);
	}
}
