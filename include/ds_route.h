#ifndef DS_ROUTE_H
#define DS_ROUTE_H

#define V 10

class route {
    private: 
        int path[V];

    public:
        route();
        int minDistance(int dist[], bool sptSet[]);
        int get_route(int** graph, int src, int dst, int** route);

};
#endif
