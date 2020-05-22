#include <stdio.h>

int main(){
	int vertecies, cost[10][10], starting_vertex, visited[10], dist[10], min, lowest_cost_vertex;
	
	printf("Enter the number of vertecies:");
	scanf("%d", &vertecies);
	printf("Enter the cost matrix:\n");
	for(int i = 0; i < vertecies; i++){
		for(int j = 0; j < vertecies; j++){
			scanf("%d", &cost[i][j]);
			if(cost[i][j] == 0){
				cost[i][j] = 999;
			}
		}
	}
	
	printf("Enter the source\n");
	scanf("%d", &starting_vertex);
	starting_vertex = starting_vertex - 1;
	
	//set the cost to move from the starting vertex
	for(int i = 0; i < vertecies; i++){
		visited[i] = 0;
		dist[i] = cost[starting_vertex][i];
	}
	//set the starting node to visited
	visited[starting_vertex] = 1;
	//remove the inf distance from the starting node to itself
	dist[starting_vertex] = 0;
	
	for(int i = 1; i < vertecies; i++){
		min = 999; //min of 999 so the first non inf number will overwrite it.
		for(int j = 0; j < vertecies; j++){
			//find the node with the lowest cost to travel to from the cuurent node
			if(dist[j] < min && visited[j] == 0){
				min = dist[j];
				lowest_cost_vertex = j;
			}
			
			//set the node with the lowest travel cost to visited
			visited[lowest_cost_vertex] = 1;
			
			//readjust distance array
			for(int j = 0; j < vertecies; j++){
				if((dist[lowest_cost_vertex] + cost[lowest_cost_vertex][j]) < dist[j]){
					dist[j] = cost[lowest_cost_vertex][j] + dist[lowest_cost_vertex];
				}
			}
		}
	}
	
	printf("Shortest paths\n");
	for(int i = 0; i < vertecies; i++){
		if(i != starting_vertex){
			printf("%d->%d = %d\n", starting_vertex, i + 1, dist[i]);
		}
	}
	return 0;
}