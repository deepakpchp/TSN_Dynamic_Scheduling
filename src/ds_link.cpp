#include <ds_link.h>
#include <iostream>
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

int link::get_waiting_slots_count(){
	return this->waiting_slots_count;
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
	this->waiting_slots_count = (HYPER_PERIOD * (NUM_OF_QUEUES_FOR_TT -1));

}

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
			this->waiting_slots_count--;
		}
		else if (link::FREE == state){
			if(link::OPEN == this->gcl[time_slot][route_queue_assignment]){
				this->open_slots_count++;
				this->slot_transmission_availablity[time_slot] = true;
			}
			else if (link::WAITING == this->gcl[time_slot][route_queue_assignment]){
				this->waiting_slots_count++;
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


int link::do_slot_allocation(int* flow_transmition_slot, int *reserved_queue_index, int period, int size){
	if (0 != (HYPER_PERIOD%period)){
		std::cerr<<"Period is not Least Common Multiple of Hyper period: HYPER_PERIOD\n";
		exit(0);
	}
 
	for (int  queue_index = QUEUES_PER_PORT-1; queue_index >= (QUEUES_PER_PORT - NUM_OF_QUEUES_FOR_TT); queue_index--){
		bool is_curr_queue_free = true;
		int slot_assignment[HYPER_PERIOD/period] = {-1};
		for (int period_index = 0; period_index < (HYPER_PERIOD / period); period_index++){
			int end_of_curr_period = (period * (period_index + 1));
			for (int time_index = flow_transmition_slot[period_index]; time_index < end_of_curr_period; time_index++){
				if (link::FREE != this->gcl[time_index][queue_index]){
					is_curr_queue_free = false;
					break;
				}
				bool is_schedulable = true;

				if(true == slot_transmission_availablity[time_index]){
					for (int size_index = 1; size_index < size; size_index++){
						if (link::FREE != this->gcl[time_index + size_index][queue_index]){
							is_curr_queue_free = false;
							break;
						}

						if(true != slot_transmission_availablity[time_index + size_index]){
							is_schedulable = false;
							break;
						}
					}
					if (true == is_curr_queue_free && true == is_schedulable){
						slot_assignment[period_index] = time_index;
						reserved_queue_index[period_index] = queue_index;
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
		}
		if (true == is_curr_queue_free){
			for (int period_index = 0; period_index < (HYPER_PERIOD / period); period_index++){
				for (int time_index = flow_transmition_slot[period_index]; time_index < slot_assignment[period_index]; time_index++){
					this->update_gcl(time_index, queue_index, link::WAITING);
				}
				for (int time_index = 0; time_index < size ; time_index++){
					this->update_gcl((time_index + slot_assignment[period_index]), queue_index, link::OPEN);
				}
				flow_transmition_slot[period_index] = slot_assignment[period_index] + 1;

			}
			break;

		}
	}

	return 0;
}
