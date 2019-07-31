#ifndef DS_NODE_H
#define DS_NODE_H

#include <ds_node_type.h>
#include <ds_link.h>

#define MAX_PORTS 10
class node {
    private:
        node_type type;
        int node_id;
        unsigned int adj_node_count;
        node* adj_node[MAX_PORTS];
		link* adj_link[MAX_PORTS];

    public:
        node(node_type, int);
        void connect(node*);
        void disconnect(int);
		void print();
        ~node();

};

#endif
