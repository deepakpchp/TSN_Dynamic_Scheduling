#include <ds.h>
#include <ds_link.h>

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
TODO
class: 
Function Name: 

Description: 

Return:
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
TODO
class: 
Function Name: 

Description: 

Return:
***************************************************************************************************/
void link::update_gcl(int time_slot, int route_queue_assignment, link::queue_reservation_state state){
	if (time_slot >= HYPER_PERIOD){
		std::cerr<<"Invalid Time Slot for reservation.\n";
		std::cerr<<"Erro link id: "<<this->get_link_id()<<" time_slot: "<<time_slot<< " route_queue_assignment: "<<route_queue_assignment<<" current state: "<<this->gcl[time_slot][route_queue_assignment]<<std::endl<<std::endl;
		exit(0);
	}
	if (state != this->gcl[time_slot][route_queue_assignment]){
		if (route_queue_assignment < (QUEUES_PER_PORT - NUM_OF_QUEUES_FOR_TT)){
			std::cerr<<"Trying to assign BE queue for TT traffic.\n";
			std::cerr<<"Erro link id: "<<this->get_link_id()<<" time_slot: "<<time_slot<< " route_queue_assignment: "<<route_queue_assignment<<" current state: "<<this->gcl[time_slot][route_queue_assignment]<<std::endl<<std::endl;
			exit(0);
		}
		if(link::OPEN == state){
			this->open_slots_count--;
			this->slot_transmission_availablity[time_slot] = false;
		}
		else if (link::WAITING == state){
			this->wait_slots_count--;
		}
		else if (link::FREE == state){
			if(link::OPEN == this->gcl[time_slot][route_queue_assignment]){
				this->open_slots_count++;
				this->slot_transmission_availablity[time_slot] = true;
			}
			else if (link::WAITING == this->gcl[time_slot][route_queue_assignment]){
				this->wait_slots_count++;
			}
		}
		else {
			std::cerr<<"Trying to assign invalid state to GCL.\n";
			std::cerr<<"Erro link id: "<<this->get_link_id()<<" time_slot: "<<time_slot<< " route_queue_assignment: "<<route_queue_assignment<<" current state: "<<this->gcl[time_slot][route_queue_assignment]<<std::endl<<std::endl;
			exit(0);

		}
		this->gcl[time_slot][route_queue_assignment] = state;
	}
	else {
		std::cerr<<"Trying to assign the gcl which is alread been assigned.\n";
		std::cerr<<"Erro link id: "<<this->get_link_id()<<" time_slot: "<<time_slot<< " route_queue_assignment: "<<route_queue_assignment<<" current state: "<<this->gcl[time_slot][route_queue_assignment]<<std::endl<<std::endl;
		exit(0);
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
