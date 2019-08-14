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

int link::get_open_slots(){
	return this->open_slots;
}

int link::get_waiting_slots(){
	return this->waiting_slots;
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
	}
	this->open_slots = HYPER_PERIOD;
	this->waiting_slots = (HYPER_PERIOD * (NUM_OF_QUEUES_FOR_TT -1));

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
			this->open_slots--;
		}
		else if (link::WAITING == state){
			this->waiting_slots--;
		}
		else if (link::FREE == state){
			if(link::OPEN == this->gcl[time_slot][route_queue_assignment]){
				this->open_slots++;
			}
			else if (link::WAITING == this->gcl[time_slot][route_queue_assignment]){
				this->waiting_slots++;
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


