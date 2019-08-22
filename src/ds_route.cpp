#include <ds_route.h>
#include <iostream>
#include <stdio.h>
#include <limits.h>

using namespace std;

// Funtion that implements Dijkstra's single source shortest path algorithm
// for a graph represented using adjacency matrix representation
/***************************************************************************************************
TODO
class: 
Function Name: 

Description: 

Return:
***************************************************************************************************/
int route::get_route(int** graph, int src, int dst, int** route)
{
	int dist[V];     // The output array.  dist[i] will hold the shortest distance from src to i
	
	bool processedSet[V]; // processedSet[i] will true if vertex i is included in shortest
							// path tree or shortest distance from src to i is finalized

	// Initialize all distances as INFINITE and processedSet[] as false, set all values in path array to -1
	for (int i = 0; i < V; i++)
	{
		dist[i] = INT_MAX, processedSet[i] = false, path[i] = -1;
	}

	// Distance of source vertex from itself is always 0
	dist[src] = 0;

	// Find shortest path for all vertices
	for (int count = 0; count < V - 1; count++)
	{
		// Pick the minimum distance vertex from the set of vertices not
		// yet processed. u is always equal to src in first iteration.
		int u = minDistance(dist, processedSet);
	
		// Mark the picked vertex as processed
		processedSet[u] = true;
		// Update list value of the adjacent vertices of the picked vertex.
		for (int v = 0; v < V; v++)
		{
			// Update dist[v] only if is not in processedSet, there is an edge from 
			// u to v, and total weight of path from src to  v through u is 
			// smaller than current value of dist[v]
			if (!processedSet[v] && graph[u][v] && dist[u] != INT_MAX && dist[u] + graph[u][v] < dist[v])
			{
				dist[v] = dist[u] + graph[u][v];
				path[v] = u;		//add vertex to path array
			}	
		}
		if (processedSet[dst] == true){
			break;
		}
	}

    *route = new int[dist[dst] + 1];
    int route_index = dist[dst];

    int next = path[dst];
    (*route)[route_index--] = dst;
    while (next != src){
        (*route)[route_index--] = next;
        next = path[next];

    }
    (*route)[route_index--] = next;
    return dist[dst] + 1;
}

// A utility function to find the vertex with minimum distance value, from
// the set of vertices not yet included in shortest path tree
/***************************************************************************************************
TODO
class: 
Function Name: 

Description: 

Return:
***************************************************************************************************/
int route::minDistance(int dist[], bool processedSet[])
{
	// Initialize min value
	int min = INT_MAX, min_index;

	for (int v = 0; v < V; v++)
		if (processedSet[v] == false && dist[v] <= min)
		{
			min = dist[v], min_index = v;
		}

	return min_index;
}

/***************************************************************************************************
TODO
class: 
Function Name: 

Description: 

Return:
***************************************************************************************************/
route::route(){
}
