#include <iostream>
#include <ds_scheduler.h>
#include <ds_node.h>
#include <ds_link.h>
#include <ds_flow.h>
#include <ds_config_reader.h>


using namespace std;

node** node_list;
flow** flow_list;
extern link* link_list[];
extern int num_of_links;

int link::id_link = 0;
int flow::id_flow = 0;

int get_link_id(int src_node_id, int dst_node_id);
int main(){

	configuration config("../input.txt");
	config.read_node_config();
	config.read_flow_config();
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
        node_list[connection[0]]->connect(node_list[connection[1]]);
    }

	for(int index = 0; index < 10; index++){
		node_list[index]->print();
	}


    flow_list = new flow*[config.get_num_of_flows()];

//	#{src_id,dst_id,dedline,size,period}
	for (int index = 0; index < config.get_num_of_flows(); index++){
		cout<<endl;
		int *flow_info = config.get_flow_info(index);
		if(NULL == flow_info){
			std::cerr << "Ignoring this flow\n";
			continue;
		}
		int src_node_id = flow_info[0];
		int dst_node_id = flow_info[1];
		int deadline = flow_info[2];
		int size = flow_info[3];
		int period = flow_info[4];
		flow_list[index] = new flow(src_node_id, dst_node_id, deadline, size, period);
		
		if(true == config.get_reservation_availability(index)){
			int route_length = config.get_route_length(index);
			int* route_nodes = config.get_route(index);
			int link_src_node_id = src_node_id;
			int* route = new int[route_length];
			link::queue_reservation_state* state = config.get_queue_state(index);
			for (int route_index = 0; route_index < route_length; route_index++){
				int link_id	= get_link_id(link_src_node_id, route_nodes[route_index]); 
				if(-1 == link_id){
					std::cerr << "Couldn't find the link for src_node_id: "<<link_src_node_id;
					std::cerr << " and dst_node_if: "<<route_nodes[route_index]<<std::endl;

				}
				route[route_index] = link_id;
				if (((route_index + 1 ) % period) == 0 ){
					link_src_node_id = src_node_id;

				}
				else if (state[route_index] == link::OPEN){
					link_src_node_id = route_nodes[route_index];
				}
				cout<<state[route_index]<<endl;
			}

			int* queue_assignment = config.get_route_queue_assignment(index);
			flow_list[index]->assign_route_and_queue(route, queue_assignment, state, route_length);

		}
	}

	for(int index = 0; index < 2; index++){
		flow_list[index]->print();
	}

	for(int index = 0; index < 10; index++){
		node_list[index]->print();
	}
	
    return 0;
}

int get_link_id(int src_node_id, int dst_node_id){
	for (int index = 0; index < num_of_links; index++){
		if((src_node_id == link_list[index]->get_src_node_id()) && (dst_node_id == link_list[index]->get_dst_node_id())){
			return index;
			cout<<index;
		}

	}
	return -1;
}
