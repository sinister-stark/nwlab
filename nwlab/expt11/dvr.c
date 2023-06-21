#include <stdio.h>
#include <limits.h>

struct node {
	int dist[100];
	int from[100];
};

int path(int src, int i, int from[]) {
	int temp;
	
	while(1) {
		temp = i;
		i = from[i];
		
		if(temp == i)
			break;
	}
	
	return temp;
}

void main() {
	int nodes;
	
	printf("Enter the number of nodes: ");
	scanf("%d", &nodes);
	int costmat[nodes][nodes];
	
	struct node rt[nodes];
	
	printf("\nEnter the cost matrix:\n");
	
	for(int i = 0; i < nodes; i++) {
		for(int j = 0; j < nodes; j++) {
			scanf("%d", &costmat[i][j]);
			
			if(costmat[i][j] == -1)
				costmat[i][j] = SHRT_MAX;
			
			rt[i].dist[j] = costmat[i][j];
			rt[i].from[j] = j;
		}
		
		costmat[i][i] = 0;
	}
	
	int count;
	
	do {
		count = 0;
		
		for(int i = 0; i < nodes; i++)
			for(int j = 0; j < nodes; j++)
            			for(int k = 0; k < nodes; k++)
					if(rt[i].dist[j] > costmat[i][k] + rt[k].dist[j]) {
						rt[i].dist[j] = rt[i].dist[k] + rt[k].dist[j];
						rt[i].from[j] = k;
						count++;
					}
        } while(count != 0);
	
	for(int i = 0; i < nodes; i++) {
		printf("\nFor router %d\n",i + 1);
		
		for(int j = 0; j < nodes; j++) {
			printf("\nShortest distance to router %d is %d via %d", j + 1, rt[i].dist[j], path(i, j, rt[i].from) + 1);
		}
        	
        	printf("\n");
        }
}
