#include <ds.h>
#include <ds_scheduler.h>
#include <ds_node.h>
#include <ds_link.h>
#include <ds_flow.h>
#include <ds_route.h>
#include <ds_config_reader.h>


using namespace std;

node** node_list;
flow** flow_list;
extern link* link_list[];
extern int num_of_links;

link*** conn_link_matrix;
int** conn_matrix; 
int g_num_of_nodes = 0;

int link::id_link = 0;
int flow::id_flow = 0;

int read_and_configure_nodes(configuration* config);
int read_and_configure_flows(configuration* config);
int get_link_id(int src_node_id, int dst_node_id);
int schedule_flow(int flow_index);
int do_reservation(flow* flow, int* route, int route_length);
void gen_tmp_conn_matrix(int** tmp_conn_matrix, flow* flow_to_scheduled);
void print_conn_link_matrix_details(configuration* config_p);


/***************************************************************************************************
TODO
class: 
Function Name: 

Description: 

Return:
***************************************************************************************************/
void print_conn_link_matrix_details(configuration* config_p){
    for (int r_index = 0; r_index < config_p->get_num_of_nodes(); r_index++){
    	for (int c_index = 0; c_index < config_p->get_num_of_nodes(); c_index++){
			if( NULL != conn_link_matrix[r_index][c_index]){
				int src_id = conn_link_matrix[r_index][c_index]->get_src_node_id();
				int dst_id = conn_link_matrix[r_index][c_index]->get_dst_node_id();
//				int open_slots = conn_link_matrix[r_index][c_index]->get_open_slots_count();
//				int waiting_slots = conn_link_matrix[r_index][c_index]->get_waiting_slots_count();
//				cout<<dst_id<<":"<<open_slots<<":"<<waiting_slots<<"\t";
				cout<<src_id<<":"<<dst_id<<"\t";
			}
			else{
				cout<<"-\t";
			}
		}
		cout<<endl;
	} 	
}

/***************************************************************************************************
TODO
class: 
Function Name: 

Description: 

Return:
***************************************************************************************************/
int read_and_configure_nodes(configuration* config){
	config->read_node_config();
	
	g_num_of_nodes = config->get_num_of_nodes();
	conn_link_matrix = new link**[config->get_num_of_nodes()];
	conn_matrix = new int*[config->get_num_of_nodes()];

	for (int index = 0; index < config->get_num_of_nodes(); index++){
		conn_link_matrix[index] = new link*[config->get_num_of_nodes()];
		conn_matrix[index] = new int[config->get_num_of_nodes()];
	}
	
    for (int r_index = 0; r_index < config->get_num_of_nodes(); r_index++){
    	for (int c_index = 0; c_index < config->get_num_of_nodes(); c_index++){
			conn_link_matrix[r_index][c_index] = NULL;
			conn_matrix[r_index][c_index] = 0;
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

/***************************************************************************************************
TODO
class: 
Function Name: 

Description: 

Return:
***************************************************************************************************/
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
			int reservation_length = config->get_reservation_length(index);
			int* route_nodes = config->get_route(index);
			int link_src_node_id = src_node_id;
			link::queue_reservation_state* state = config->get_queue_state(index);

			int* route_links = new int[reservation_length];

			for (int route_index = 0; route_index < reservation_length; route_index++){
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
			int* assigned_time_slot = config->get_assigned_time_slot(index);

			flow_list[index]->assign_route_and_queue(assigned_time_slot, 
					route_links, queue_assignment, state, reservation_length);

		}
	}
	return 0;
}

/***************************************************************************************************
TODO
class: 
Function Name: 

Description: 

Return:
***************************************************************************************************/
int get_link_id(int src_node_id, int dst_node_id){

	for (int index = 0; index < num_of_links; index++){
		if((src_node_id == link_list[index]->get_src_node_id()) && (dst_node_id == link_list[index]->get_dst_node_id())){
			return index;
		}

	}
	return -1;
}

/***************************************************************************************************
TODO
class: 
Function Name: 

Description: 

Return:
***************************************************************************************************/
int delete_flow_reservation(int flow_index){

	flow_list[flow_index]->remove_route_and_queue_assignment();
	return 0;
}

/***************************************************************************************************
TODO
class: 
Function Name: 

Description: 

Return:
***************************************************************************************************/
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
	
	for (int index = 0; index < config.get_num_of_flows(); index++){
		delete_flow_reservation(index);
	}

	print_conn_link_matrix_details(&config);

	for(int index = 0; index < config.get_num_of_flows(); index++){
		flow_list[index]->print();
	}

	for(int index = 0; index < config.get_num_of_nodes(); index++){
		node_list[index]->print();
	}

	for(int index = 0; index < config.get_num_of_flows(); index++){
		if(!flow_list[index]->get_is_scheduled()){
			int ret_val = 0;
			ret_val = schedule_flow(index);
			if (SUCCESS != ret_val){
				ERROR("Unable to schedule the below mentioned flow\n");
				flow_list[index]->print();
			}
			else {
				INFO("Successfully scheduled the below mentioned flow");
				flow_list[index]->print();
			}
		}
	}

	for(int index = 0; index < config.get_num_of_nodes(); index++){
		node_list[index]->print();
	}

    return 0;
}


/***************************************************************************************************
TODO
class: 
Function Name: 

Description: 

Return:
***************************************************************************************************/
void gen_tmp_conn_matrix(int** tmp_conn_matrix, flow* flow_to_scheduled){
	for (int r_index=0; r_index < g_num_of_nodes; r_index++){
		for (int c_index=0; c_index < g_num_of_nodes; c_index++){
			if(NULL !=  conn_link_matrix[r_index][c_index]){
				tmp_conn_matrix[r_index][c_index] = 1;
			}
			else{

				tmp_conn_matrix[r_index][c_index] = 0;
			}
		}
	}
}


/***************************************************************************************************
TODO
class: 
Function Name: 

Description: 

Return:
***************************************************************************************************/
int schedule_flow(int flow_index){
	flow* flow_to_schedule = flow_list[flow_index];

	//int* route = get_route();
    // int route[2][5] = {{8, 7, 3, 1, 0},
    // {9, 7, 3, 5, 6}};


	int** tmp_conn_matrix =  new int*[g_num_of_nodes] ;
	for (int index = 0; index < g_num_of_nodes; index++){
		tmp_conn_matrix[index] = new int[g_num_of_nodes];
	}

	gen_tmp_conn_matrix(tmp_conn_matrix, flow_to_schedule);
    route router;
    int* route_t = NULL;
	int src_node_id = flow_to_schedule->get_src_node_id();
	int dst_node_id = flow_to_schedule->get_dst_node_id();

    int route_length = 0;
    route_length = router.get_route(tmp_conn_matrix, src_node_id, dst_node_id, &route_t );
	int ret_val = 0;
	ret_val = do_reservation(flow_to_schedule, route_t, route_length);

	if (SUCCESS != ret_val){
		return FAILURE;
	}


    return SUCCESS;
}


/***************************************************************************************************
TODO
class: 
Function Name: 

Description: 

Return:
***************************************************************************************************/
int do_reservation(flow* flow, int* route, int route_length){
	int period = flow->get_period();
	int size = flow->get_size();
	int deadline = flow->get_deadline();
	
	int reservation_detail[route_length-1][HYPER_PERIOD/period];
	int reserved_queue_index[route_length-1][HYPER_PERIOD/period];
	int* flow_transmition_slot = new int[HYPER_PERIOD/period];
	int old_flow_transmition_slot[route_length-1][HYPER_PERIOD/period];

	for (int period_index = 0; period_index < (HYPER_PERIOD / period); period_index++){
		flow_transmition_slot[period_index] = (period * (period_index));
	}

	for (int node_index = 0; node_index < (route_length - 1); node_index++){
		link* link_p = conn_link_matrix[route[node_index]][route[node_index+1]];

		for (int period_index = 0; period_index < (HYPER_PERIOD / period); period_index++){
			old_flow_transmition_slot[node_index][period_index] = flow_transmition_slot[period_index];
		}
		int ret_val;

		ret_val = link_p->do_slot_allocation(flow_transmition_slot, 
				reserved_queue_index[node_index], size, period, deadline);

		if (SUCCESS != ret_val){
			LOG("Unnable to allocate resource on the link:"<<link_p->get_link_id()<<
				" for the flow: "<<flow->get_flow_id());
			return FAILURE;
		}

		for (int period_index = 0; period_index < (HYPER_PERIOD / period); period_index++){
			reservation_detail[node_index][period_index] = flow_transmition_slot[period_index];
		}
		
	}

	int assigned_time_slot[HYPER_PERIOD];
	int queue_assignment[HYPER_PERIOD];
	link::queue_reservation_state state[HYPER_PERIOD];
	int route_nodes[HYPER_PERIOD];
	int reservation_length = 0;


	for (int period_index = 0; period_index < (HYPER_PERIOD / period); period_index++){
		for (int node_index = 0; node_index <  (route_length - 1); node_index++){
			for (int index = 1; index < reservation_detail[node_index][period_index] - old_flow_transmition_slot[node_index][period_index]; index++){
				assigned_time_slot[reservation_length] = (old_flow_transmition_slot[node_index][period_index] + index -1);
				route_nodes[reservation_length] = get_link_id(route[node_index], route[node_index+1]);
				queue_assignment[reservation_length] = reserved_queue_index[node_index][period_index];
				state[reservation_length] = link::WAITING;
				reservation_length++;
			}

			for(int index = 0; index < size; index++){
				assigned_time_slot[reservation_length] = ( reservation_detail[node_index][period_index] + index -1);
				route_nodes[reservation_length] = get_link_id(route[node_index], route[node_index+1]);
				queue_assignment[reservation_length] = reserved_queue_index[node_index][period_index];
				state[reservation_length] = link::OPEN;
				reservation_length++;
			}
		}
	}
	flow->assign_route_and_queue(assigned_time_slot, route_nodes, queue_assignment, state, reservation_length);
    return SUCCESS;
}
