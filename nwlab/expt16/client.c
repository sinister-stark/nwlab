#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

typedef struct queue {
	int* arr;
	int FRONT;
	int REAR;
	int SIZE;
	int REMAINING;
} queue;

void initQueue(queue* q, int size) {
	q->FRONT = -1;
	q->REAR = -1;
	q->SIZE = size;
	q->REMAINING = size;
	q->arr = malloc(size * sizeof(int));
}

void enqueue(queue* q, int i) {
	if(q->FRONT == -1)
		q->FRONT = 0;
	else if((q->REAR + 1) % q->SIZE == q->FRONT)
		return;
	
	q->REAR = (q->REAR + 1) % q->SIZE;
	q->arr[q->REAR] = i;
}

int dequeue(queue* q) {
	int data;
	
	if(q->FRONT == -1)
		return -1;
	else if(q->FRONT == q->REAR) {
		data = q->arr[q->FRONT];
		q->FRONT = -1;
		q->REAR = -1;
	} else {
		data = q->arr[q->FRONT];
		q->FRONT = (q->FRONT + 1) % q->SIZE;
	}
	
	q->REMAINING++;
	return data;
}

void sendData(int client_fd, queue* q, int size) {
	int k = 0, n;
	
	while(1) {
		printf("\n");
		for(int i = 0; i < size; i++) {
			n = dequeue(q);
			
			if(n == -1)
				return;
			else if(n == 0)
				k++;
			
			if(send(client_fd, &n, sizeof(int), 0) < 0) {
				printf("Send failed!\n");
				exit(1);
			} else {
				printf("Sent packet %d: %d\n", k, n);
			}
		}
		
		sleep(1);
	}
}

void main(int argc, char* argv[]) {
	int PORT, client_fd;
	struct sockaddr_in serv_addr;
	
	printf("Enter port: ");
	scanf("%d", &PORT);
	
	client_fd = socket(AF_INET, SOCK_STREAM, 0);
	
	if(client_fd < 0) {
		printf("Socket creation failed!\n");
		exit(1);
	}
	
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = INADDR_ANY;
	serv_addr.sin_port = htons(PORT);
	
	if(connect(client_fd, (struct sockaddr*) &serv_addr, sizeof(serv_addr)) < 0) {
		printf("Connection failed!\n");
		exit(1);
	}
	
	queue q;
	int num, size;
	
	printf("Enter bucket size: ");
	scanf("%d", &size);
	
	initQueue(&q, size);
	
	printf("Enter packet size to send per second: ");
	scanf("%d", &size);
	
	while(1) {
		printf("\nEnter number of packets to send: ");
		scanf("%d", &num);
		
		int packets[num];
		
		for(int i = 0; i < num; i++) {
			printf("Enter packet %d size: ", i + 1);
			scanf("%d", &packets[i]);
		}
		
		for(int i = 0; i < num; i++) {
			if(q.REMAINING < packets[i]) {
				printf("\nBucket full! Packet %d rejected!\n", i + 1);
				continue;
			}
			
			for(int j = 0; j < packets[i]; j++) {
				enqueue(&q, j);
				q.REMAINING--;
			}
		}
		
		sendData(client_fd, &q, size);
	}
	
	close(client_fd);
}
