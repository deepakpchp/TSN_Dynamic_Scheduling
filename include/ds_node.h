#ifndef DS_NODE_H
#define DS_NODE_H

#include <ds_link.h>

#define MAX_NODES 10
#define MAX_PORTS 10

class node {
    private:
        int node_id;
        unsigned int adj_node_count;
        node* adj_node[MAX_PORTS];
		link* adj_link[MAX_PORTS];
		linked_list passing_flow_list;

    public:
        enum  node_type{ 
            END_SYSTEM, 
            NETWORK_SWITCH 
        };
        node_type type;
        node(node_type, int);
        void connect(node*);
        void disconnect(int);
		void print();
		int get_node_id();
		int get_adj_node_count();

		void add_passing_flow_to_list(int flow_id);
		int delete_passing_flow_from_list(int flow_id);
		int get_passing_flow_ids(int **passing_flow_ids);

        ~node();

};

#endif
