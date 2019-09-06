#include <ds_utils.h>
#include <ds_node.h>
#include <ds_link.h>
#include <ds_flow.h>

extern node** node_list;
extern flow* get_flow_ptr_from_id(int flow_id);
extern link*** conn_link_matrix;

void link::set_link_id(int link_id){
	this->link_id = link_id;
}
int link::get_link_id(){
	return this->link_id;
}
void link::set_src_node_id(int src_node_id){
	this->src_node_id = src_node_id;
}
int link::get_src_node_id(){
	return this->src_node_id;
}

int link::get_open_slots_count(){
	return this->open_slots_count;
}

int link::get_wait_slots_count(){
	return this->wait_slots_count;
}

void link::set_dst_node_id(int dst_node_id){
	this->dst_node_id = dst_node_id;
}
int link::get_dst_node_id(){
	return this->dst_node_id;
}

void link::set_gcl(int* gcl, int time_slot){
	for(int index = 0; index < QUEUES_PER_PORT; index++){
		this->gcl[time_slot][index] = gcl[index];
	}
}
int** link::get_gcl(){
	return this->gcl;
}

/***************************************************************************************************
class: link
Function Name: constructor

Description: Constructor function for the class link. 

Return: None
***************************************************************************************************/
link::link(int src_node_id, int dst_node_id){
	this->link_id = this->id_link++;
	this->src_node_id = src_node_id;
	this->dst_node_id = dst_node_id;
	this->gcl = new int*[HYPER_PERIOD];
	for (int index = 0; index < HYPER_PERIOD; index++){
		this->gcl[index] = new int[QUEUES_PER_PORT];
		for (int index2 = 0; index2 < QUEUES_PER_PORT; index2++){
			this->gcl[index][index2] = FREE;
		}
		this->slot_transmission_availablity[index] = true;
	}
	this->open_slots_count = HYPER_PERIOD;
	this->wait_slots_count = (HYPER_PERIOD * (NUM_OF_QUEUES_FOR_TT -1));
}

/***************************************************************************************************
class: link
Function Name: destructor

Description: destructor function for the class link. 

Return: None
***************************************************************************************************/
link::~link(){
	int* flow_ids = NULL;
	int num_of_flows =  this->get_passing_flow_ids(&flow_ids);
	std::cout<<"In link destructor num_of_flows:"<<num_of_flows<<" link_id:"<<this->get_link_id();
	std::cout<<"\n";
	for (int index = 0; index < num_of_flows; index++){
		flow* flow_to_delete = get_flow_ptr_from_id(flow_ids[index]);
		if (NULL != flow_to_delete){
			flow_to_delete->remove_route_and_queue_assignment();
		}
	}

    int src_node_id = this->get_src_node_id();
    int dst_node_id = this->get_dst_node_id();
    conn_link_matrix[src_node_id][dst_node_id] = nullptr;
    conn_link_matrix[dst_node_id][src_node_id] = nullptr;

	delete(flow_ids);
}
/***************************************************************************************************
class: link
Function Name: 

Description: This function will assign the route_queue_assignment and queue state for the respective
			 time slot on the links. This funcion will also update the flow id's of the flows 
			 passing through the link.

Return: None
***************************************************************************************************/
void link::update_gcl(int time_slot, int route_queue_assignment, int flow_id, 
		link::queue_reservation_state state){
	if (time_slot >= HYPER_PERIOD){
		FATAL("Invalid Time Slot reservation for flow:"<<flow_id<<".\nError link id:"
			<<this->get_link_id()<<" time_slot:"<<time_slot<< " route_queue_assignment:"
			<<route_queue_assignment<<" state:"<<this->gcl[time_slot][route_queue_assignment]);
	}
	if (state != this->gcl[time_slot][route_queue_assignment]){
		if (route_queue_assignment < (QUEUES_PER_PORT - NUM_OF_QUEUES_FOR_TT)){
			FATAL("Trying to assign BE queue for TT traffic.\nErro link id: "<<this->get_link_id()
				<<" time_slot: "<<time_slot<< " route_queue_assignment: "<<route_queue_assignment
				<<" current state: "<<this->gcl[time_slot][route_queue_assignment]);
		}
		if(link::OPEN == state){
			this->open_slots_count--;
			this->slot_transmission_availablity[time_slot] = false;
			this->add_passing_flow_to_list(flow_id);
		}
		else if (link::WAITING == state){
			this->wait_slots_count--;
			this->add_passing_flow_to_list(flow_id);
		}
		else if (link::FREE == state){
			int ret_val = 0;
			ret_val = this->delete_passing_flow_from_list(flow_id);
			if(SUCCESS != ret_val){
				LOG("Trying to remove the flow which is not passing through the link.\n"
					<<"link id:"<<this->get_link_id()<<" invalid flow id to delete:"<<flow_id);
			}

			if(link::OPEN == this->gcl[time_slot][route_queue_assignment]){
				this->open_slots_count++;
				this->slot_transmission_availablity[time_slot] = true;
			}
			else if (link::WAITING == this->gcl[time_slot][route_queue_assignment]){
				this->wait_slots_count++;
			}
		}
		else {
			FATAL("Trying to assign invalid state to GCL.\nErro link id: "<<this->get_link_id()
				<<" time_slot: "<<time_slot<< " route_queue_assignment: "<<route_queue_assignment
				<<" current state:"<<this->gcl[time_slot][route_queue_assignment]);
		}
		this->gcl[time_slot][route_queue_assignment] = state;
	}
	else {
		FATAL("Trying to assign the gcl which is alread been assigned.\nErro link id: "
			<<this->get_link_id()<<" time_slot: " <<time_slot
			<< " route_queue_assignment:"<<route_queue_assignment
			<<" current state:"<<this->gcl[time_slot][route_queue_assignment]);
	}

}


/***************************************************************************************************
class: link
Function Name: do_slot_allocation

Description: For a given link, this function will try to allocate the slots of size "size" and for 
			 every cycle of lenght "period" from a given time slot in "flow_transmition_slot" for 
			 each period.

Return: 0 - Successful, 1 Failure
***************************************************************************************************/
int link::do_slot_allocation(int* flow_transmition_slot, int *reserved_queue_index, 
		int size, int period, int deadline){
	
	/*Sanity check to see if the HYPERPERIOD is divisible by period*/
	if (0 != (HYPER_PERIOD%period)){
		int hyper_period = HYPER_PERIOD;
		FATAL("Hyper period:"<<hyper_period<<" is  not divisible by period:"<<period<<std::endl);
	}

	int lowest_tt_queue =  (QUEUES_PER_PORT - NUM_OF_QUEUES_FOR_TT);
	
	/*Try to do reservation from highest TT queue to Lowest TT queue*/
	for (int  queue_index = QUEUES_PER_PORT-1; queue_index >= lowest_tt_queue; queue_index--){
		int num_of_succ_reservation = 0;
		bool is_curr_queue_free = true;
		int slot_assignment[HYPER_PERIOD/period] = {-1};

		/*For a given queue in queue_index, try to find free slots for all the period */
		for (int period_index = 0; period_index < (HYPER_PERIOD / period); period_index++){
			
			int start_time_index = flow_transmition_slot[period_index];
			int end_time_index = (period * period_index) + deadline;
			
			for (int time_index = start_time_index; time_index < end_time_index; time_index++){

				int time_index_t = time_index % HYPER_PERIOD;
				/*If the time slot is not free then this queue cannot be used */
				if (link::FREE != this->gcl[time_index_t][queue_index]){
					is_curr_queue_free = false;
					break;
				}

				bool is_schedulable = true;

				/*Check if this slot is available for transmission or occupied by some other flow*/
				if(true == this->slot_transmission_availablity[time_index_t]){

					/*If the slot is available for transmission, then check if the consecutive time 
					 slots of same queue are available for transmission for all the frames of this
					 instance of the flow which will be of length size.*/
					for (int size_index = 1; size_index < size; size_index++){

						int time_slot_index = (time_index_t + size_index) % HYPER_PERIOD;
						if (link::FREE != this->gcl[time_slot_index][queue_index]){
							is_curr_queue_free = false;
							break;
						}

						if(true != this->slot_transmission_availablity[time_slot_index]){
							is_schedulable = false;
							break;
						}
					}

					if (true == is_curr_queue_free && true == is_schedulable){
						slot_assignment[period_index] = time_index_t;
						reserved_queue_index[period_index] = queue_index;
						num_of_succ_reservation++;
						break;
					}
				}

				if (false == is_curr_queue_free){
					break;
				}

			}

			if (false == is_curr_queue_free){
				break;
			}

			/*If unnable to allocate for a period on a queue then it doesn't make sense to continue 
			 searching for other periods on the same queue*/
			if (period_index >= num_of_succ_reservation){
				break;
			}
		}
	
		if (true == is_curr_queue_free && (num_of_succ_reservation == (HYPER_PERIOD/period))){

			for (int period_index = 0; period_index < (HYPER_PERIOD / period); period_index++){
				flow_transmition_slot[period_index] = slot_assignment[period_index] + 1;
			}
			return SUCCESS;
		}
	}
	LOG("Unnable to Do the reservation");
	return FAILURE;
}


/***************************************************************************************************
class: link
Function Name: add_passing_flow_to_list

Description: this function will add the flow id to the list of passing flows though calling link obj

Return: None
***************************************************************************************************/
void link::add_passing_flow_to_list(int flow_id){
	/*Add the flow to the list of passing flows on the link only if it doesnt already exist*/
	this->passing_flow_list.add_no_duplicate(flow_id);
	node* src_node = node_list[this->get_src_node_id()];
	node* dst_node = node_list[this->get_dst_node_id()];

	src_node->add_passing_flow_to_list(flow_id);
	dst_node->add_passing_flow_to_list(flow_id);
}

/***************************************************************************************************
class: link
Function Name: delete_passing_flow_from_list

Description: this function will delete the flow id from the list of passing flows though 
			 calling link obj

Return: 0 - Successful, 1 Failure
***************************************************************************************************/
int link::delete_passing_flow_from_list(int flow_id){
	node* src_node = node_list[this->get_src_node_id()];
	node* dst_node = node_list[this->get_dst_node_id()];

	src_node->delete_passing_flow_from_list(flow_id);
	dst_node->delete_passing_flow_from_list(flow_id);

	return this->passing_flow_list.remove(flow_id);
}


/***************************************************************************************************
class: link
Function Name: get_passing_flow_count

Description: Returns number of flows passing through this link

Return: 0 - Successful, 1 Failure
***************************************************************************************************/
int link::get_passing_flow_count(){
	return this->passing_flow_list.get_count();
}

/***************************************************************************************************
class: link
Function Name: get_passing_flow_ids

Description: Returns all the flow ids passing through this link in a array passed

Return: 0 - Successful, 1 Failure
***************************************************************************************************/
int link::get_passing_flow_ids(int **passing_flow_ids){
	return this->passing_flow_list.get_all_data(passing_flow_ids);
}
