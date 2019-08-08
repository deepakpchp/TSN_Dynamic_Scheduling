#include <iostream>
#include <ds_scheduler.h>
#include <ds_node.h>
#include <ds_link.h>
#include <ds_flow.h>
#include <ds_config_reader.h>


using namespace std;

node** node_list;
int link::id_link = 0;
int flow::id_flow = 0;
int main(){

	configuration config("../input.txt");
	config.read_configuration();
    node_list = new node*[config.get_num_of_nodes()];
    for (int index = 0; index < config.get_num_of_nodes(); index++){
        auto node_type = config.get_node_type(index); 
        if  (node_type == "ES"){
            node_list[index] = new node(node::END_SYSTEM, index);
        } else if (node_type == "NS" ){
            node_list[index] = new node(node::NETWORK_SWITCH, index);
        }
        else{
            cout<<"In else ";
            cout<<node_type<<endl;
        }

    }


    for (int index = 0; index < config.get_num_of_connection(); index++){
        int* connection = config.get_connection(index);
        cout<<connection[0]<<connection[1];
        node_list[connection[0]]->connect(node_list[connection[1]]);
    }

	for(int index = 0; index < 10; index++){
		node_list[index]->print();
	}

	flow* flow_list[3];
	flow_list[0] = new flow(node_list[8]->get_node_id(), node_list[0]->get_node_id(), 4, 2, 4);
	flow_list[1] = new flow(node_list[8]->get_node_id(), node_list[0]->get_node_id(), 6, 2, 8);

	int route[4] = {17, 13, 5, 1};
	int route_queue_assignment[4] = {7, 7, 7, 7};
	link::queue_reservation_state state[4] = {link::OPEN, link::OPEN, link::OPEN, link::OPEN};
	flow_list[0]->assign_route_and_queue(route, route_queue_assignment, state, 4);

	for(int index = 0; index < 2; index++){
		flow_list[index]->print();
	}

	for(int index = 0; index < 10; index++){
		node_list[index]->print();
	}
	
    return 0;
}


