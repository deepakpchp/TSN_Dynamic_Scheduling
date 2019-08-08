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

	node_list[0] = new node(node_type::END_SYSTEM, 0);
	node_list[1] = new node(node_type::NETWORK_SWITCH, 1);
	node_list[2] = new node(node_type::END_SYSTEM, 2);
	node_list[3] = new node(node_type::NETWORK_SWITCH, 3);
	node_list[4] = new node(node_type::END_SYSTEM, 4);
	node_list[5] = new node(node_type::NETWORK_SWITCH, 5);
	node_list[6] = new node(node_type::END_SYSTEM, 6);
	node_list[7] = new node(node_type::NETWORK_SWITCH, 7);
	node_list[8] = new node(node_type::END_SYSTEM, 8);
	node_list[9] = new node(node_type::END_SYSTEM, 9);

	node_list[0]->connect(node_list[1]);
	node_list[1]->connect(node_list[2]);
	node_list[1]->connect(node_list[3]);
	node_list[3]->connect(node_list[4]);
	node_list[3]->connect(node_list[5]);
	node_list[3]->connect(node_list[7]);
	node_list[5]->connect(node_list[6]);
	node_list[7]->connect(node_list[8]);
	node_list[7]->connect(node_list[9]);
	node_list[7]->connect(node_list[5]);






	for(int index = 0; index < 10; index++){
		node_list[index]->print();
	}

	flow* flow_list[3];
	flow_list[0] = new flow(node_list[8]->get_node_id(), node_list[0]->get_node_id(), 4, 2, 4);
	flow_list[1] = new flow(node_list[8]->get_node_id(), node_list[0]->get_node_id(), 6, 2, 8);

	int route[5] = {8, 7, 3, 1, 9};
	int route_queue_assignment[5] = {7, 7, 7, 7, 7};
	link::queue_reservation_state state[5] = {link::OPEN, link::OPEN, link::OPEN, link::OPEN, link::OPEN};
	flow_list[0]->assign_route_and_queue(route, route_queue_assignment, state, 5);

	for(int index = 0; index < 2; index++){
		flow_list[index]->print();
	}

	for(int index = 0; index < 10; index++){
		node_list[index]->print();
	}

//	flow_list[0]

	return 0;
}


