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
}

void link::update_gcl(int time_slot, int route_queue_assignment, link::queue_reservation_state state){
	this->gcl[time_slot][route_queue_assignment] = state;
}

