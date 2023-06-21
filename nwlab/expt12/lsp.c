#include <limits.h>
#include <stdio.h>
#include <stdbool.h>

int minDistance(int V, int dist[], bool sptSet[]) {
	int min = INT_MAX, min_index;
	
	for (int v = 0; v < V; v++)
		if (sptSet[v] == false && dist[v] <= min) {
			min = dist[v];
			min_index = v;
		}
	
	return min_index;
}

int path(int src, int i, int from[]) {
	int temp = from[i];
	
	if(temp == src) {
		return i;
	}
	
	i = from[i];
	
	while(i != src) {
		temp = i;
		i = from[i];
	}
	
	return temp;
}

void printSolution(int V, int dist[], int from[], int src) {
	printf("\nFor router %d\n", src + 1);
	printf("\nTo router\tShortest distance\tVia\n");
	
	for (int i = 0; i < V; i++)
		printf("%d\t\t%d\t\t\t%d\n", i + 1, dist[i], path(src, i, from) + 1);
}

void dijkstra(int V, int costmat[V][V], int src) {
	int dist[V];
	int from[V];
	
	bool sptSet[V];
	
	for(int i = 0; i < V; i++) {
		dist[i] = INT_MAX;
		from[i] = i;
		sptSet[i] = false;
	}
	
	dist[src] = 0;
	
	for(int count = 0; count < V - 1; count++) {
		int u = minDistance(V, dist, sptSet);
		
		sptSet[u] = true;
		
		for(int v = 0; v < V; v++)
			if(!sptSet[v] && costmat[u][v] && dist[u] != INT_MAX && dist[u] + costmat[u][v] < dist[v]) {
				from[v] = u;
				dist[v] = dist[u] + costmat[u][v];
			}
	}
	
	printSolution(V, dist, from, src);
}

void main() {
	int nodes;
	
	printf("Enter the number of nodes: ");
	scanf("%d", &nodes);
	int costmat[nodes][nodes];
	
	printf("\nEnter the cost matrix:\n");
	
	for(int i = 0; i < nodes; i++) {
		for(int j = 0; j < nodes; j++) {
			scanf("%d", &costmat[i][j]);
			if(costmat[i][j] == -1)
				costmat[i][j] = SHRT_MAX;
		}
		
		costmat[i][i] = 0;
	}
	
	for(int i = 0; i < nodes; i++)
		dijkstra(nodes, costmat, i);
}
