#include <ds_scheduler.h>

node** node_list;
flow** flow_list;
extern egress_link* link_list[];
extern int num_of_links;

egress_link*** conn_link_matrix;
int** conn_matrix; 
int g_num_of_nodes = 0;
int g_num_of_flows = 0;

int egress_link::id_link = 0;
int flow::id_flow = 0;


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
	conn_link_matrix = new egress_link**[config->get_num_of_nodes()];
	conn_matrix = new int*[config->get_num_of_nodes()];

	for (int index = 0; index < config->get_num_of_nodes(); index++){
		conn_link_matrix[index] = new egress_link*[config->get_num_of_nodes()];
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

	g_num_of_flows = config->get_num_of_flows();
    flow_list = new flow*[MAX_NUM_FLOWS];
	for (int index = 0; index < MAX_NUM_FLOWS; index++){
		flow_list[index] = nullptr;
	}

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
			egress_link::queue_reservation_state* state = config->get_queue_state(index);

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
				else if (state[route_index] == egress_link::OPEN){
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
		if (nullptr != link_list[index]){
			if((src_node_id == link_list[index]->get_src_node_id()) && 
					(dst_node_id == link_list[index]->get_dst_node_id())){
				return index;
			}
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
int delete_flow(int flow_index){
	if (nullptr == flow_list[flow_index]){
		return 0;
	}
	flow_list[flow_index]->remove_route_and_queue_assignment(flow::DELETE_FLOW);
	delete(flow_list[flow_index]);
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
			egress_link* link_p = conn_link_matrix[r_index][c_index];

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
class: 
Function Name: get_link_to_detele_in_cm

Description: During prcess of route discovery, new route is discovered by removing a worst link on
			 alredy discovered route. This Function will return a worst link along the route which
			 will be deleted by the calling function.

Return: pointer to the link to be deleted from the route
***************************************************************************************************/
egress_link*  get_link_to_detele_in_cm(int* route, int route_length, egress_link* link_not_to_delete){
	
	egress_link* tmp_link[route_length] = {NULL};
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

	egress_link* ret_link = NULL;
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
	egress_link* link_not_to_delete = NULL;
	egress_link* link_to_delete = NULL;;
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
class:  
Function Name: schedule_flow

Description: This function will try to schedule the flow with id flow_id (passed as parameter).
			 Step1: Generate a new connectivity matrix with only required links.
			 Step2: Discover k-shortest path using the new connectivity matrix
			 Step3: Rank the flows based on various attributes.
			 Step4: Try to schedule from the best ranked route. If Successful then return otherwise 
			 		try next ranked route until all the k-paths are exaused. If none of them can be
					scheduled then return FAILURE

Return: Return: 0 - Successful, 1 Failure 
***************************************************************************************************/
int schedule_flow(int flow_id){

	flow* flow_to_schedule = nullptr;
	for (int index = 0; index < MAX_NUM_FLOWS; index++){
		if (nullptr == flow_list[index]){
			continue;
		}
		if (flow_id == flow_list[index]->get_flow_id()){
			flow_to_schedule = flow_list[index];
		}
	}
	

	if (nullptr == flow_to_schedule){
		ERROR("Trying to schedule Flow id:"<<flow_id<<" which doesn't exist");
		return FAILURE;
	}

	if (flow::SCHEDULED == flow_to_schedule->get_reservation_status()){
		INFO("Flow already scheduled");
		std::cout<<"++++++++++++++++++++++++++++++\n";
		return SUCCESS;
	}

	if (flow::DELETE_FLOW == flow_to_schedule->get_reservation_status()){
		INFO("Trying to schedule a flow which is ment to be deleted");
		return FAILURE;
	}

	int** tmp_conn_matrix =  new int*[g_num_of_nodes] ;
	for (int index = 0; index < g_num_of_nodes; index++){
		tmp_conn_matrix[index] = new int[g_num_of_nodes];
	}

	/*Step-1 Generate a temp connectivity matrix*/
	gen_tmp_conn_matrix(tmp_conn_matrix, flow_to_schedule);
	int src_node_id = flow_to_schedule->get_src_node_id();
	int dst_node_id = flow_to_schedule->get_dst_node_id();

	int** k_paths = NULL;
	int route_length[K_SHORTEST_PATH] = {0};

	int num_of_paths = 0;
	/*Step-2 Discover the k shortest paths */
	num_of_paths = get_k_shortest_paths(tmp_conn_matrix, src_node_id, dst_node_id, 
			&k_paths, route_length);

	/*Step-3 Rank the discoverd paths*/
	rank_flows(k_paths, route_length, num_of_paths);

	/*Step-4 Perform the reservation*/
	for (int index = 0; index < num_of_paths; index++){
		int ret_val = 0;
//		ret_val = perform_flow_reservation(flow_to_schedule, k_paths[index], route_length[index]);
		ret_val = perform_flow_reservation_inverse(flow_to_schedule, k_paths[index], route_length[index]);
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
			 attributes of the route(path) such as average load on the egress_links along the path, 
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
			egress_link* link_p = conn_link_matrix[src_node_index][dst_node_index];

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
		}

		rank[route_index] =   (avg_open_slots[route_index]/max_open_slots) * -1
							+ (avg_wait_slots[route_index]/max_wait_slots) * 2
							+ (((float)(route_length[0]-1)/(route_length[route_index]- 1))) * 1; 
	}
#if 1
	/*Sort the routes based on the ranks calculated in previous step*/
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
class: 
Function Name: perform_flow_reservation_inverse

Description: Once the routes are discovered, this method is called to do slot reservation on all
			 the links throughout the route of the flows.

Return: Return: 0 - Successful, 1 Failure 
***************************************************************************************************/
int perform_flow_reservation_inverse(flow* flow, int* route, int route_length){
	int period = flow->get_period();
	int size = flow->get_size();
	int deadline = flow->get_deadline();

	/*To store the transmission time slot on each link for each period*/
	int slot_reservation_detail[route_length-1][HYPER_PERIOD/period];
	/*To store the queue assignment on each link for each period*/
	int reserved_queue_index[route_length-1][HYPER_PERIOD/period];
	int* flow_transmition_slot = new int[HYPER_PERIOD/period];
	int old_flow_transmition_slot[route_length-1][HYPER_PERIOD/period];

	/*Start from the end of deadline and allocate in reverse order*/
	for (int period_index = 0; period_index < (HYPER_PERIOD / period); period_index++){
		flow_transmition_slot[period_index] = ((period * (period_index)) + deadline) -1;
	}

	/*Try to perform reservation on all the links along the given route from the last link*/
	for (int node_index = (route_length - 2); node_index >= 0 ; node_index--){
		egress_link* link_p = conn_link_matrix[route[node_index]][route[node_index+1]];

		for (int period_index = 0; period_index < (HYPER_PERIOD / period); period_index++){
			old_flow_transmition_slot[node_index][period_index] 
				= flow_transmition_slot[period_index];
		}
		int ret_val;

		/*Perform the reservation on the link*/
		ret_val = link_p->do_slot_allocation_inverse(flow_transmition_slot, 
				reserved_queue_index[node_index], size, period, deadline);
		if (SUCCESS != ret_val){
			LOG("Unnable to allocate resource on the link:"<<link_p->get_link_id()<<
				" for the flow: "<<flow->get_flow_id());
			return FAILURE;
		}

		/*Check if the start of the flow can be at the begging of the period if the reservation is 
		  done on remaining links*/
		for (int period_index = 0; period_index < (HYPER_PERIOD / period); period_index++){

			int best_case_start_slot  
				= flow_transmition_slot[period_index] - node_index;

			if(0 > best_case_start_slot){
				LOG("Unnable to meet the deadline:"<<deadline<<" for the flow:"<<flow->get_flow_id()
				    <<" best case scenario for the start time was @:"
					<<best_case_start_slot);
				return FAILURE;

			}
		}

		for (int period_index = 0; period_index < (HYPER_PERIOD / period); period_index++){
			slot_reservation_detail[node_index][period_index] = (flow_transmition_slot[period_index]
																- (size - 2)) % HYPER_PERIOD;
		}
		
	}

	/*Once the reservation on all the links along the route is successful, update the corresponding 
	 flow and the Gate Control List*/
	int assigned_time_slot[deadline * size * (HYPER_PERIOD/period)];
	int queue_assignment[deadline * size * (HYPER_PERIOD/period)];
	egress_link::queue_reservation_state state[deadline * size * (HYPER_PERIOD/period)];
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

				state[reservation_length] = egress_link::WAITING;
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

				state[reservation_length] = egress_link::OPEN;
				reservation_length++;
			}
		}
	}
	
	flow->assign_route_and_queue(assigned_time_slot
			, route_nodes, queue_assignment, state, reservation_length);

    return SUCCESS;
}

/***************************************************************************************************
class: 
Function Name: perform_flow_reservation 

Description: Once the routes are discovered, this method is called to do slot reservation on all
			 the links throughout the route of the flows.

Return: Return: 0 - Successful, 1 Failure 
***************************************************************************************************/
int perform_flow_reservation(flow* flow, int* route, int route_length){
	int period = flow->get_period();
	int size = flow->get_size();
	int deadline = flow->get_deadline();

	/*Variables to store the details on each link for each period*/

	/*To store the transmission time slot on each link for each period*/
	int slot_reservation_detail[route_length-1][HYPER_PERIOD/period];
	/*To store the queue assignment on each link for each period*/
	int reserved_queue_index[route_length-1][HYPER_PERIOD/period];
	int* flow_transmition_slot = new int[HYPER_PERIOD/period];
	int old_flow_transmition_slot[route_length-1][HYPER_PERIOD/period];

	for (int period_index = 0; period_index < (HYPER_PERIOD / period); period_index++){
		flow_transmition_slot[period_index] = (period * (period_index));
	}

	/*Try to perform reservation on all the links along the given route*/
	for (int node_index = 0; node_index < (route_length - 1); node_index++){
		egress_link* link_p = conn_link_matrix[route[node_index]][route[node_index+1]];

		for (int period_index = 0; period_index < (HYPER_PERIOD / period); period_index++){
			old_flow_transmition_slot[node_index][period_index] 
				= flow_transmition_slot[period_index];
		}
		int ret_val;

		/*Perform the reservation on the link*/
		ret_val = link_p->do_slot_allocation(flow_transmition_slot, 
				reserved_queue_index[node_index], size, period, deadline);
		if (SUCCESS != ret_val){
			LOG("Unnable to allocate resource on the link:"<<link_p->get_link_id()<<
				" for the flow: "<<flow->get_flow_id());
			return FAILURE;
		}

		/*Check if the deadline can be met after reservation on remaining links*/
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

		for (int period_index = 0; period_index < (HYPER_PERIOD / period); period_index++){
			slot_reservation_detail[node_index][period_index] = flow_transmition_slot[period_index];
		}
		
	}

	/*Once the reservation on all the links along the route is successful, update the corresponding 
	 flow and the Gate Control List*/
	int assigned_time_slot[deadline * size * (HYPER_PERIOD/period)];
	int queue_assignment[deadline * size * (HYPER_PERIOD/period)];
	egress_link::queue_reservation_state state[deadline * size * (HYPER_PERIOD/period)];
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

				state[reservation_length] = egress_link::WAITING;
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

				state[reservation_length] = egress_link::OPEN;
				reservation_length++;
			}
		}
	}
	
	flow->assign_route_and_queue(assigned_time_slot
			, route_nodes, queue_assignment, state, reservation_length);

    return SUCCESS;
}

/***************************************************************************************************
TODO
class: 
Function Name: 

Description: 

Return:
***************************************************************************************************/

int delete_node(int node_id){
	node* node_to_delete = node_list[node_id];

	if (NULL == node_to_delete){
		ERROR("Node id:"<<node_id<<" trying to delete doestnt exist.");
	}

	int* flow_ids = NULL;
	int num_of_flows =  node_to_delete->get_passing_flow_ids(&flow_ids);
	
	for (int index = 0; index < num_of_flows; index++){
		flow* flow_to_delete = get_flow_ptr_from_id(flow_ids[index]);
		cout<<"Flow ID:"<<flow_to_delete->get_flow_id();
		if (NULL != flow_to_delete){
			flow_to_delete->remove_route_and_queue_assignment(flow::NODE_DELETED);
		}
	}
	
	delete(node_to_delete);
	delete(flow_ids);
	return SUCCESS;
}

/***************************************************************************************************
TODO
class: 
Function Name: get_flow_ptr_from_id 

Description: 

Return:
***************************************************************************************************/
flow* get_flow_ptr_from_id(int flow_id){
	for (int index = 0; index < MAX_NUM_FLOWS; index++){
		if( NULL != flow_list[index]){
			if(flow_id == flow_list[index]->get_flow_id()){
				return flow_list[index];
			}
		}
	}

	//ERROR("Trying to retrive the flow with flow_id:"<<flow_id<<" which doesnt exist.");
	return NULL;
}

/***************************************************************************************************
TODO
class: 
Function Name: dynamic_scheduling 

Description: This function will run in a infinite loop. It listens for tcp connections on the 
			 configured port. On receiving a connections, it accepts the connection and receive the
			 notification and writes it into to a temp file notification.txt. Then it calls the 
			 notification handler to process the notification. Once that is done it does some 
			 cleanup and returns to listning to new connections.

Return: This function will run till a stop request is received. On receiving stop request, cleanup 
		is done and it eill return.
***************************************************************************************************/
int dynamic_scheduling(){

	int port = 8080;
	int server_fd, new_socket, valread; 
	struct sockaddr_in address; 
	//    int opt = 1; 
	int addrlen = sizeof(address); 
	char buffer[FILE_READ_BUFFER_SUZE] = {0}; 

	/*Creating socket file descriptor*/
	if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) 
	{ 
		perror("socket failed"); 
		exit(EXIT_FAILURE); 
	} 

	address.sin_family = AF_INET; 
	address.sin_addr.s_addr = INADDR_ANY; 
	address.sin_port = htons( port ); 

	if (bind(server_fd, (struct sockaddr *)&address,  
				sizeof(address))<0) 
	{ 
		perror("bind failed"); 
		exit(EXIT_FAILURE); 
	} 


	while(1){
		if (listen(server_fd, 3) < 0) 
		{ 
			perror("listen"); 
			exit(EXIT_FAILURE); 
		} 
		if ((new_socket = accept(server_fd, (struct sockaddr *)&address,  
						(socklen_t*)&addrlen))<0){ 
			perror("accept"); 
			exit(EXIT_FAILURE); 
		}
	
		char *connected_ip = inet_ntoa(address.sin_addr);

		const char* notification_file_name = "notification.txt";
		if(check_if_file_exist(notification_file_name)){
			if( remove( notification_file_name) != 0 ){
				perror( "Error deleting file" );
				FATAL("Cleanup Error!!! Notification sortage file found but unnable to delete");
			}
		}

		std::ofstream myFile;
		myFile.open(notification_file_name);

		while (1){
			memset(buffer,0, FILE_READ_BUFFER_SUZE);
			valread = read( new_socket , buffer, FILE_READ_BUFFER_SUZE); 
			if (0 >= valread){
				std::cout<<"Received junk from notifier\n";
				perror("Receive");
				break;
			}
			myFile<<buffer;

			char *output = NULL;
			output = strstr (buffer, "EOF");
			if(output) {
				break;
			}
			output = strstr (buffer, "STOP_REQUEST");
			if(output) {
				int ret_val = 0;
				const char *response_msg = "Disconnect"; 
				ret_val = send(new_socket , response_msg , strlen(response_msg) , 0 ); 

				if (-1 == ret_val){
					std::cout<<"Sending error message failed\n";
					perror("Send");
				}

				close(new_socket);
				if(check_if_file_exist(notification_file_name)){
					if( remove( notification_file_name) != 0 )
						perror( "Error deleting file" );
				}
				cleanup();
				std::cout<<"STOP_REQUEST received. Stopping dynamic scheduling gracefully\n";
				return SUCCESS;
			}
		}

		myFile.close();

		int ret_val = 0;
		const char *response_msg = "Disconnect"; 
		ret_val = send(new_socket , response_msg , strlen(response_msg) , 0 ); 

		if (-1 == ret_val){
			std::cout<<"Sending error message failed\n";
			perror("Send");
		}

		close(new_socket);

		std::cout<<"New notification received from IP:"<<connected_ip<<" Processing the same\n";
		process_notification();
		if(check_if_file_exist(notification_file_name)){
			if( remove( notification_file_name) != 0 )
				perror( "Error deleting file" );
			else
				puts( "File successfully deleted" );
		}

	}
	printf("Exiting \n");

	return 0; 

}       

/***************************************************************************************************
TODO
class: 
Function Name: 

Description: 

Return:
***************************************************************************************************/
int process_notification(){
	notification_handler notification_handler_obj;
	notification_handler_obj.read_modification_request();
	notification_handler_obj.print();
	notification_handler_obj.process_notification();
	std::vector<flow*> flows_to_schedule;

	for (int index = 0; index < MAX_NUM_FLOWS; index++){
		if (nullptr == flow_list[index]){
			continue;
		}
		if(flow::SCHEDULED != flow_list[index]->get_reservation_status() &&
				flow::DELETE_FLOW != flow_list[index]->get_reservation_status()){
			flows_to_schedule.push_back( flow_list[index]);
		}
	}

	for (unsigned int index = 0; index < flows_to_schedule.size(); index++){

		std::cout<<"Flow to schedule id:"<<flows_to_schedule[index]->get_flow_id()<<"\n";

		int ret_val = 0;
		ret_val = schedule_flow(flows_to_schedule[index]->get_flow_id());
		if (SUCCESS != ret_val){
			ERROR("Unnable to schedule the below mentioned flow\n");
			flows_to_schedule[index]->print();
		}
		else {
			INFO("Successfully scheduled the flow_id: "<<flow_list[index]->get_flow_id());
//			flows_to_schedule[index]->print();
		}
	}

	for(unsigned int index = 0; index < flows_to_schedule.size(); index++){
		if (NULL != flow_list[index]){
			flows_to_schedule[index]->print();
		}
	}
	for(int index = 0; index < g_num_of_nodes; index++){
		if (NULL != node_list[index]){
			node_list[index]->print();
		}
	}

	return SUCCESS;
}

/***************************************************************************************************
class: 
Function Name: cleanup() 

Description: This function is called before the Dynamic scheduling is called to release all the 
			 Dynamically allocated memory and release all the resource allocated for the binary

Return:
***************************************************************************************************/
void cleanup(){
	std::cout<<"Cleanup finished\n";
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

	if (read_and_configure_flows(&config)){
		cerr<<"Something went wrong in read_and_configure_flows.\nExiting the program\n";
		exit (0);
	}


	for (int index = 0; index < config.get_num_of_flows(); index++){
		if (NULL != flow_list[index]){
			flow_list[index]->print();
		}
	}

	for (int index = 0; index < config.get_num_of_nodes(); index++){
		if (NULL != node_list[index]){
			node_list[index]->print();
		}
	}

	for (int index = 0; index < config.get_num_of_flows(); index++){
		if (flow::DELETE_FLOW == flow_list[index]->get_reservation_status()){
			delete_flow(index);
		}
	}

	print_conn_link_matrix_details(&config);

	for(int index = 0; index < MAX_NUM_FLOWS; index++){
		if (nullptr == flow_list[index]){
			continue;
		}

		if(flow::SCHEDULED != flow_list[index]->get_reservation_status() && 
				flow::DELETE_FLOW != flow_list[index]->get_reservation_status()){
			int ret_val = 0;
			ret_val = schedule_flow(flow_list[index]->get_flow_id());
			if (SUCCESS != ret_val){
				ERROR("Unnable to schedule the below mentioned flow\n");
				flow_list[index]->print();
			}
			else {
				INFO("Successfully scheduled the flow_id: "<<flow_list[index]->get_flow_id());
			}
		}
	}

	cout<<endl;
	for(int index = 0; index < config.get_num_of_flows(); index++){
		if (NULL != flow_list[index]){
			flow_list[index]->print();
		}
	}

	for(int index = 0; index < config.get_num_of_nodes(); index++){
		if (NULL != node_list[index]){
			node_list[index]->print();
		}
	}

	std::cout<<"Initial Configurations completed.\n";
	std::cout<<"\n#############################################################################\n";
	std::cout<<"#############################################################################\n\n";
	std::cout<<"Starting Dynamic Scheduling Mode\n";

	int ret_val = -1;
	ret_val = dynamic_scheduling();

	if (FAILURE == ret_val){
		ERROR("Something went wrong in Dynamic scheduling");
	}

	std::cout<<"Dynamic scheduling Stopped\n";
	return SUCCESS;
}

