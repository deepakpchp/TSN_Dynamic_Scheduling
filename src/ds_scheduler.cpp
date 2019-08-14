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

link*** conn_matrix;


int link::id_link = 0;
int flow::id_flow = 0;

int read_and_configure_nodes(configuration* config);
int read_and_configure_flows(configuration* config);
int get_link_id(int src_node_id, int dst_node_id);

int read_and_configure_nodes(configuration* config){
	config->read_node_config();

	conn_matrix = new link**[config->get_num_of_nodes()];
	for (int index = 0; index < config->get_num_of_nodes(); index++){
		conn_matrix[index] = new link*[config->get_num_of_nodes()];
	}
	
    for (int r_index = 0; r_index < config->get_num_of_nodes(); r_index++){
    	for (int c_index = 0; c_index < config->get_num_of_nodes(); c_index++){
			conn_matrix[r_index][c_index] = NULL;
		}
	} 	

    node_list = new node*[config->get_num_of_nodes()];
    for (int index = 0; index < config->get_num_of_nodes(); index++){
        auto node_type = config->get_node_type(index); 
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

    for (int index = 0; index < config->get_num_of_connection(); index++){
        int* connection = config->get_connection(index);
        node_list[connection[0]]->connect(node_list[connection[1]]);

    }
	return 0;
}

int read_and_configure_flows(configuration* config){
	config->read_flow_config();
    flow_list = new flow*[config->get_num_of_flows()];

	for (int index = 0; index < config->get_num_of_flows(); index++){

		int *flow_info = config->get_flow_info(index);
		if(NULL == flow_info){
			std::cerr << "Ignoring this flow\n";
			continue;
		}

		/*{src_id,dst_id,dedline,size,period}*/
		int src_node_id = flow_info[0];
		int dst_node_id = flow_info[1];
		int deadline = flow_info[2];
		int size = flow_info[3];
		int period = flow_info[4];
		flow_list[index] = new flow(src_node_id, dst_node_id, deadline, size, period);

		if(true == config->get_reservation_availability(index)){
			int route_length = config->get_route_length(index);
			int* route_nodes = config->get_route(index);
			int link_src_node_id = src_node_id;
			link::queue_reservation_state* state = config->get_queue_state(index);

			int* route_links = new int[route_length];

			for (int route_index = 0; route_index < route_length; route_index++){
				int link_id	= get_link_id(link_src_node_id, route_nodes[route_index]); 
				if(-1 == link_id){
					std::cerr << "Couldn't find the link for src_node_id: "<<link_src_node_id;
					std::cerr << " and dst_node_if: "<<route_nodes[route_index]<<std::endl;
					return -1;

				}
				route_links[route_index] = link_id;
				if (((route_index + 1 ) % period) == 0 ){
					link_src_node_id = src_node_id;

				}
				else if (state[route_index] == link::OPEN){
					link_src_node_id = route_nodes[route_index];
				}
			}

			int* queue_assignment = config->get_route_queue_assignment(index);
			flow_list[index]->assign_route_and_queue(route_links, queue_assignment, state, route_length);

		}
	}
	return 0;
}

int get_link_id(int src_node_id, int dst_node_id){
	for (int index = 0; index < num_of_links; index++){
		if((src_node_id == link_list[index]->get_src_node_id()) && (dst_node_id == link_list[index]->get_dst_node_id())){
			return index;
		}

	}
	return -1;
}

int delete_flow_reservation(int flow_index){
	flow_list[flow_index]->remove_route_and_queue_assignment();
	return 0;
}

int main(){

	configuration config("../input.txt");

	if (read_and_configure_nodes(&config)){
		cerr<<"Something went wrong in read_and_configure_nodes.\nExiting the program\n";
		exit (0);
	}

	if(read_and_configure_flows(&config)){
		cerr<<"Something went wrong in read_and_configure_flows.\nExiting the program\n";
		exit (0);
	}


	for(int index = 0; index < config.get_num_of_flows(); index++){
		flow_list[index]->print();
	}

	for(int index = 0; index < config.get_num_of_nodes(); index++){
		node_list[index]->print();
	}
/*	
	for (int index = 0; index < config.get_num_of_flows()-1; index++){
		delete_flow_reservation(index);
	}
*/
	cout<<endl<<endl;
	for(int index = 0; index < config.get_num_of_flows(); index++){
		flow_list[index]->print();
	}

	for(int index = 0; index < config.get_num_of_nodes(); index++){
		node_list[index]->print();
	}

	cout<<endl;
    for (int r_index = 0; r_index < config.get_num_of_nodes(); r_index++){
    	for (int c_index = 0; c_index < config.get_num_of_nodes(); c_index++){
			if( NULL != conn_matrix[r_index][c_index]){
				int src_id = conn_matrix[r_index][c_index]->get_src_node_id();
				int dst_id = conn_matrix[r_index][c_index]->get_dst_node_id();
				int open_slots = conn_matrix[r_index][c_index]->get_open_slots();
				int waiting_slots = conn_matrix[r_index][c_index]->get_waiting_slots();
				
				cout<<dst_id<<":"<<open_slots<<":"<<waiting_slots<<"\t";
			}
			else{
				cout<<"-\t";
			}
		}
		cout<<endl<<endl;
	} 	

    return 0;
}

