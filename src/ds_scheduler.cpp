#include <iostream>
#include <ds_scheduler.h>
#include <ds_node.h>
#include <ds_link.h>
#include <ds_flow.h>
using namespace std;



int link::id_link = 0;
int flow::id_flow = 0;
int main(){
	node* node_list[20];

	for(int index = 0; index < 3; index++){
		node_list[index] = new node(node_type::END_SYSTEM, index);
	}

	node_list[0]->connect(node_list[1]);
	node_list[0]->connect(node_list[2]);
	node_list[1]->connect(node_list[2]);

	for(int index = 0; index < 3; index++){
		node_list[index]->print();
	}

	flow* flow_list[10];
	for(int index = 0; index < 3; index++){
		flow_list[index] = new flow(node_list[index]->get_node_id(), node_list[(index+1)%3]->get_node_id(), 6, 2);
	}

	for(int index = 0; index < 3; index++){
		flow_list[index]->print();
	}

	return 0;
}


