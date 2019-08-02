#include <iostream>
#include <ds_flow.h>
#include <ds_link.h>

using namespace std;
flow::flow(int src_node_id, int dst_node_id, int deadline, int size){
	this->flow_id = this->id_flow++;
	this->src_node_id = src_node_id;
	this->dst_node_id = dst_node_id;
	this->deadline = deadline;
	this->size = size;
	this->route = new int[HYPER_PERIOD];
	this->route_queue_assignment = new int[HYPER_PERIOD] ;
	for (int index = 0; index < HYPER_PERIOD; index++){
		this->route_queue_assignment[index] = -1;
		this->route[index] = -1;
	}
	this->route_length = 0;
	this->is_scheduled = false;
}
void flow::set_is_scheduled(bool is_scheduled){
	this->is_scheduled = is_scheduled;
}

void flow::set_src_node_id(int src_node_id){
	this->src_node_id = src_node_id;
}

void flow::set_dst_node_id(int dst_node_id){
	this->dst_node_id = dst_node_id;
}

void flow::set_deadline(int deadline){
	this->deadline = deadline;
}

void flow::set_size(int size){
	this->size = size;
}

void flow::set_route(int *route, int route_length){
	this->route_length = route_length;
	for (int index = 0; index < route_length; index++){
		this->route[index] = route[index];
	}
}

void flow::set_route_queue_assignment(int *route_queue_assignment, int route_length){
	this->route_length = route_length;
	for (int index = 0; index < route_length; index++){
		this->route_queue_assignment[index] = route_queue_assignment[index];
	}
}

int flow::get_flow_id(){
	return this->flow_id;
}

bool flow::get_is_scheduled(){
	return this->is_scheduled;
}

int flow::get_src_node_id(){
	return this->src_node_id;
}

int flow::get_dst_node_id(){
	return this->dst_node_id;
}

int flow::get_deadline(){
	return this->deadline;
}

int flow::get_size(){
	return this->size;
}

int* flow::get_route(){
	return this->route;
}

int* flow::get_route_queue_assignment() {
	return this->route_queue_assignment;
}

int flow::get_route_length(){
	return this->route_length;
}

void flow::print(){
	cout<<"Flow_id :"<<this->get_flow_id()<<endl;
	cout<<this->src_node_id<<" -> "<<this->dst_node_id<<" Scheduled_Status:"<<this->get_is_scheduled()<<" Deadline:"<<this->get_deadline()<<" Size:"<<this->get_size()<<endl;
	if(this->get_is_scheduled()){
		int route_length = this->get_route_length();
		int *route = this->get_route();
		int *route_queue_assignment = this->get_route_queue_assignment();
		cout<<"Route Length : "<<route_length<<" Route: ";
		for (int index = 0; index < this->route_length; index ++){
			cout<<route[index]<<"q("<<route_queue_assignment[index]<<") "<<endl;
		}
		
	}
	cout<<endl;
}

void flow::assign_route_and_queue(int *route, int *route_queue_assignment, queue_reservation_state state[], int route_length){
	this->set_route(route, route_length);
	this->set_route_queue_assignment(route_queue_assignment, route_length);

	for(int index = 0; index < route_length; index++){
		if (route[-1 != index]){
			link_list[route[index]].update_glc(index, route_queue_assignment[index], state[index]) 
		}
	}



}
