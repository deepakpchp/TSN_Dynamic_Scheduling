#include <iostream>
#include <ds_scheduler.h>
#include <ds_node.h>
#include <ds_link.h>
using namespace std;



int link::id = 0;
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
	return 0;
}


