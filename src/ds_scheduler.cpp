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
int get_k_shortest_paths(int** conn_matrix, int src_id, int dst_id, int*** route, 
		int route_length[]);
link*  get_link_to_detele_in_cm(int* route, int route_length, link* link_not_to_delete);
void rank_flows(int** route, int* route_length, int num_of_paths);


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
//				int wait_slots = conn_link_matrix[r_index][c_index]->get_wait_slots_count();
//				cout<<dst_id<<":"<<open_slots<<":"<<wait_slots<<"\t";
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
					std::cerr << " and dst_node_id: "<<route_nodes[route_index]<<std::endl;
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
		if((src_node_id == link_list[index]->get_src_node_id()) && 
				(dst_node_id == link_list[index]->get_dst_node_id())){
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
				ERROR("Unnable to schedule the below mentioned flow\n");
				flow_list[index]->print();
			}
			else {
				INFO("Successfully scheduled the flow_id: "<<flow_list[index]->get_flow_id());
//				flow_list[index]->print();
			}
		}
	}
	
	cout<<endl;
	for(int index = 0; index < config.get_num_of_flows(); index++){
		flow_list[index]->print();
	}

	for(int index = 0; index < config.get_num_of_nodes(); index++){
		node_list[index]->print();
	}

    return 0;
}


/***************************************************************************************************
class: 
Function Name: gen_tmp_conn_matrix 

Description: This function will generate a temporary connectivity matrix with only the links that 
			 can satisify the requirments of the flow passed. 
			 This functions also removes the links to the nodes which has only one adjecent node 
			 and it is not source or destintion. By doing this the worl load on the route discovery 
			 algorithm is reduced.

Return: Void. As a reference new connectivity matrix is updated.
***************************************************************************************************/
void gen_tmp_conn_matrix(int** tmp_conn_matrix, flow* flow_to_be_scheduled){
	/* 2) Remove all the links that dont have enough free slots*/
	/* 3) TODO */
	for (int r_index=0; r_index < g_num_of_nodes; r_index++){
		for (int c_index=0; c_index < g_num_of_nodes; c_index++){
			link* link_p = conn_link_matrix[r_index][c_index];

			if(NULL ==  conn_link_matrix[r_index][c_index]){
				tmp_conn_matrix[r_index][c_index] = 0;
				continue;
			}

			/*Remove the link if it does't have enough resource for the flow_to_be_scheduled */
			if (link_p->get_open_slots_count() < flow_to_be_scheduled->get_size()){
				tmp_conn_matrix[r_index][c_index] = 0;
				continue;
			}

			/*Don't remove the link if one of the end of the link is connecting to src or dst node*/
			if (link_p->get_src_node_id() == flow_to_be_scheduled->get_src_node_id() 
			|| link_p->get_dst_node_id() == flow_to_be_scheduled->get_src_node_id() 
			|| link_p->get_src_node_id() == flow_to_be_scheduled->get_dst_node_id() 
			|| link_p->get_dst_node_id() == flow_to_be_scheduled->get_dst_node_id()){
				tmp_conn_matrix[r_index][c_index] = 1;
				continue;
			}

			node* src_node = node_list[link_p->get_src_node_id()];
			node* dst_node = node_list[link_p->get_dst_node_id()];

			/*Delete the links which are connecting to end nodes which are not src or dst nodes*/
			if ((src_node->get_adj_node_count() == 1 || dst_node->get_adj_node_count() == 1)){
				tmp_conn_matrix[r_index][c_index] = 0;
			}
			else{
				tmp_conn_matrix[r_index][c_index] = 1;

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
link*  get_link_to_detele_in_cm(int* route, int route_length, link* link_not_to_delete){
	
	link* tmp_link[route_length] = {NULL};
	int num_of_links = 0;

	for (int index = 0; index < route_length-1; index++){
		int src_node_id = route[index];
		int dst_node_id = route[index+1];
		
		node* src_node = node_list[src_node_id];
		node* dst_node = node_list[dst_node_id];
		if (index == 0 && 1 == src_node->get_adj_node_count()){
			continue;
		}

		if (index == route_length-2 && 1 == dst_node->get_adj_node_count()){
			continue;
		}

		tmp_link[num_of_links++] = conn_link_matrix[src_node_id][dst_node_id];
		
	}

	link* ret_link = NULL;
	for (int index = 0; index < num_of_links; index++){
		if (NULL == ret_link && tmp_link[index] != link_not_to_delete){
			ret_link = tmp_link[index];
			continue;
		}
		if((ret_link->get_open_slots_count() < tmp_link[index]->get_open_slots_count())
			&& tmp_link[index] != link_not_to_delete){
			ret_link = tmp_link[index];
		}
	}

	return ret_link;
}

/***************************************************************************************************
Function Name: get_k_shortest_paths

Description: This function will return K_SHORTEST_PATH shortest paths for source node to destination
			 node from the given connectivity matrix. For every new path discovery, a most desired 
			 link will be deleted in the connectivity matrix.

Return: Number of discovered paths if any otherwise 0
***************************************************************************************************/
int get_k_shortest_paths(int** conn_matrix, int src_id, int dst_id, int*** k_paths, 
		int route_length[]){

	route router;
	*k_paths = new int*[K_SHORTEST_PATH];
	for (int index = 0; index < K_SHORTEST_PATH; index++){
		(*k_paths)[index] = NULL;
	}

	int num_of_paths = 0;

	int src_node_id = -1;
	int dst_node_id = -1; 
	link* link_not_to_delete = NULL;
	link* link_to_delete = NULL;;
	for (int index = 0; index < K_SHORTEST_PATH; index++){
		int ret_val = router.get_route(conn_matrix, src_id, dst_id, &(*k_paths)[index]);
		if (ret_val <= 0){
			if (0 == index){
				ERROR("No path with enough resource exist between source:"<<src_id<<
						" and dst"<<dst_id);
				return num_of_paths;
			}

			link_not_to_delete = link_to_delete;
		}
		else{
			link_not_to_delete = NULL;
			route_length[num_of_paths++] = ret_val;
		}
			
		/*This function will return the a link which is most desirable to delete in the discovred 
		  route based on the configured weightage of diffrent parameters such as number of free 
		  slots
		  most number of free consecutive slots etc.*/
		link_to_delete = get_link_to_detele_in_cm((*k_paths)[index], route_length[index]
				, link_not_to_delete);
		/*Update the conn matrix*/
		
		if (NULL == link_to_delete){
			break;
		}

		src_node_id = link_to_delete->get_src_node_id();
		dst_node_id = link_to_delete->get_dst_node_id();
		

		conn_matrix[src_node_id][dst_node_id] = 0;
	}
	return num_of_paths;
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
	int src_node_id = flow_to_schedule->get_src_node_id();
	int dst_node_id = flow_to_schedule->get_dst_node_id();

	int** k_paths = NULL;
	int route_length[K_SHORTEST_PATH] = {0};

	int num_of_paths = 0;
	/*Get the k shortest paths */
	num_of_paths = get_k_shortest_paths(tmp_conn_matrix, src_node_id, dst_node_id, 
			&k_paths, route_length);

	rank_flows(k_paths, route_length, num_of_paths);
//	for(int index = 0; index < 3; index++){
//		flow_list[index]->print();
//	}
//	cout<<"Num of paths: "<<num_of_paths<<endl;
	for (int index = 0; index < num_of_paths; index++){
		int ret_val = 0;
		ret_val = do_reservation(flow_to_schedule, k_paths[index], route_length[index]);
		if(SUCCESS == ret_val){
			for (int tmp_index = index; tmp_index < K_SHORTEST_PATH; tmp_index++){
				delete(k_paths[tmp_index]);

			}
			delete(k_paths);
			return SUCCESS;
		}

		delete(k_paths[index]);
	}

	for (int index = num_of_paths; index < K_SHORTEST_PATH; index++){
		delete(k_paths[index]);
	}

	delete(k_paths);

	return FAILURE;
}

/***************************************************************************************************
TODO
class: 
Function Name: rank_flows

Description: This functions will sort the routes based on the ranks calculated using various 
			 attributes of the route(path) such as average load on the links along the path, 
			 route length etc. 

Return: None
***************************************************************************************************/
void rank_flows(int** route, int* route_length, int num_of_routes){

	float rank[num_of_routes];
	float avg_open_slots[num_of_routes] = {0.0};
	float avg_wait_slots[num_of_routes] = {0.0};
	float max_open_slots = 0.0;
	float max_wait_slots = 0.0;

	for (int route_index = 0; route_index < num_of_routes; route_index++){
		for (int node_index = 0; node_index < route_length[route_index] - 1; node_index++){
	
			int src_node_index = route[route_index][node_index];
			int dst_node_index = route[route_index][node_index+1];
			link* link_p = conn_link_matrix[src_node_index][dst_node_index];

			float open_slot_count = (float)link_p->get_open_slots_count();
			float wait_slot_count = (float)link_p->get_wait_slots_count();

			avg_open_slots[route_index] += open_slot_count;
			avg_wait_slots[route_index] += wait_slot_count;

			if(max_open_slots < open_slot_count){
				max_open_slots = open_slot_count;
			}

			if(max_wait_slots < wait_slot_count){
				max_wait_slots = wait_slot_count;
			}

		}

		avg_open_slots[route_index] /= (route_length[route_index] - 1);
		avg_wait_slots[route_index] /= (route_length[route_index] - 1);

	}

	for (int route_index = 0; route_index < num_of_routes; route_index++){
		for (int node_index = 0; node_index < route_length[route_index] - 1; node_index++){
//			cout<<route[route_index][node_index]<<"->";
		}

		rank[route_index] =   (avg_open_slots[route_index]/max_open_slots) * -1
							+ (avg_wait_slots[route_index]/max_wait_slots) * 2
							+ (((float)(route_length[0]-1)/(route_length[route_index]- 1))) * 1; 
//		cout<<rank[route_index]<<endl;
	}
#if 1
	for (int i = 0; i < num_of_routes-1; i++){
		for (int j = i+1; j < num_of_routes; j++){
			if (rank[i] < rank[j]){
				float tmp = rank[i];
				rank[i] = rank[j];
				rank[j] = tmp;

				int* tmp_route = route[i];
				route[i] = route[j];
				route[j] = tmp_route;

				int tmp_route_length = route_length[i];
				route_length[i] = route_length[j];
				route_length[j] = tmp_route_length;

			}
		}
	}
#endif
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
	
	int slot_reservation_detail[route_length-1][HYPER_PERIOD/period];
	int reserved_queue_index[route_length-1][HYPER_PERIOD/period];
	int* flow_transmition_slot = new int[HYPER_PERIOD/period];
	int old_flow_transmition_slot[route_length-1][HYPER_PERIOD/period];

	for (int period_index = 0; period_index < (HYPER_PERIOD / period); period_index++){
		flow_transmition_slot[period_index] = (period * (period_index));
	}

	/*Try to perform reservation on all the links along the given route*/
	for (int node_index = 0; node_index < (route_length - 1); node_index++){
		link* link_p = conn_link_matrix[route[node_index]][route[node_index+1]];

		for (int period_index = 0; period_index < (HYPER_PERIOD / period); period_index++){
			old_flow_transmition_slot[node_index][period_index] 
				= flow_transmition_slot[period_index];
		}
		int ret_val;

		ret_val = link_p->do_slot_allocation(flow_transmition_slot, 
				reserved_queue_index[node_index], size, period, deadline);
		
		for (int period_index = 0; period_index < (HYPER_PERIOD / period); period_index++){
			int best_case_deadline 
				= flow_transmition_slot[period_index] + (route_length - 1) - (node_index +1);
			if(best_case_deadline > (deadline + (period * period_index))){
				LOG("Unnable to meet the deadline:"<<deadline<<" for the flow:"<<flow->get_flow_id()
				    <<" best case scenario was of first frame reaching dst was @:"
					<<best_case_deadline);
				return FAILURE;

			}
		}
		if (SUCCESS != ret_val){
			LOG("Unnable to allocate resource on the link:"<<link_p->get_link_id()<<
				" for the flow: "<<flow->get_flow_id());
			return FAILURE;
		}

		for (int period_index = 0; period_index < (HYPER_PERIOD / period); period_index++){
			slot_reservation_detail[node_index][period_index] = flow_transmition_slot[period_index];
		}
		
	}

	/*Once the reservation on all the links along the route is successful, update the corresponding 
	 flow and the Gate Control List*/
	int assigned_time_slot[deadline * size * (HYPER_PERIOD/period)];
	int queue_assignment[deadline * size * (HYPER_PERIOD/period)];
	link::queue_reservation_state state[deadline * size * (HYPER_PERIOD/period)];
	int route_nodes[deadline * size * (HYPER_PERIOD/period)];
	int reservation_length = 0;


	for (int period_index = 0; period_index < (HYPER_PERIOD / period); period_index++){
		for (int node_index = 0; node_index <  (route_length - 1); node_index++){
			for (int index = 1; index < slot_reservation_detail[node_index][period_index] 
					- old_flow_transmition_slot[node_index][period_index]; index++){

				assigned_time_slot[reservation_length] = 
					(old_flow_transmition_slot[node_index][period_index] + index -1) % HYPER_PERIOD;

				route_nodes[reservation_length] 
					= get_link_id(route[node_index], route[node_index+1]);

				queue_assignment[reservation_length] 
					= reserved_queue_index[node_index][period_index];

				state[reservation_length] = link::WAITING;
				reservation_length++;
			}

			/*TODO Should all the instance of Transmission be stored in flow or only the first 
			  instance of the Transmission in each link ?*/
//			for(int index = 0; index < 1; index++)
			for(int index = 0; index < size; index++){

				assigned_time_slot[reservation_length] 
					= (slot_reservation_detail[node_index][period_index] + index -1) % HYPER_PERIOD;

				route_nodes[reservation_length] 
					= get_link_id(route[node_index], route[node_index+1]);

				queue_assignment[reservation_length] 
					= reserved_queue_index[node_index][period_index];

				state[reservation_length] = link::OPEN;
				reservation_length++;
			}
		}
	}
	
	flow->assign_route_and_queue(assigned_time_slot
			, route_nodes, queue_assignment, state, reservation_length);

    return SUCCESS;
}
