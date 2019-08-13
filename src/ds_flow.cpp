#include <iostream>
#include <ds_flow.h>

extern link* link_list[];
using namespace std;
flow::flow(int src_node_id, int dst_node_id, int deadline, int size, int period){
	this->flow_id = this->id_flow++;
	this->src_node_id = src_node_id;
	this->dst_node_id = dst_node_id;
	this->deadline = deadline;
	this->size = size;
	this->period = period;
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

void flow::set_period(int period){
	this->period = period;
}

void flow::set_route(int *route, int route_length){
	this->route_length = route_length;
	this->route = new int[this->route_length];
	for (int index = 0; index < route_length; index++){
		this->route[index] = route[index];
	}
}

void flow::set_state(link::queue_reservation_state *state, int route_length){
	this->route_length = route_length;
	this->state = new link::queue_reservation_state[this->route_length];
	for (int index = 0; index < route_length; index++){
		this->state[index] = state[index];
	}
}



void flow::set_route_queue_assignment(int *route_queue_assignment, int route_length){
	this->route_length = route_length;
	this->route_queue_assignment = new int[this->route_length];
	for (int index = 0; index < route_length; index++){
		this->route_queue_assignment[index] = route_queue_assignment[index];
	}
	this->set_is_scheduled(true);
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

int flow::get_period(){
	return this->period;
}

int* flow::get_route(){
	return this->route;
}

int* flow::get_route_queue_assignment() {
	return this->route_queue_assignment;
}

link::queue_reservation_state* flow::get_state(){
	return this->state;
}

int flow::get_route_length(){
	return this->route_length;
}


void flow::delete_route(){
	this->route_length = 0;
	delete(this->route);
}

void flow::delete_state(){
	this->route_length = 0;
	delete(this->state);
}

void flow::delete_route_queue_assignment(){
	this->route_length = 0;
	delete(this->route_queue_assignment);
	this->set_is_scheduled(false);
}

void flow::print(){
	cout<<"Flow_id: "<<this->get_flow_id()<<endl;
	cout<<"From node: "<<this->src_node_id<<" to "<<this->dst_node_id<<endl;
	cout<<"Deadline: "<<this->get_deadline()<<", Size: "<<this->get_size()<<", Period: "<<this->get_period() <<endl;
    cout<<"Scheduled_Status: "<<this->get_is_scheduled()<<endl;
	if(this->get_is_scheduled()){
		int route_length = this->get_route_length();
		int *route = this->get_route();
		int *route_queue_assignment = this->get_route_queue_assignment();
		cout<<"Route Length: "<<route_length<<endl<<"Route: ";
		for (int index = 0; index < this->route_length; index ++){
			cout<<route[index]<<":q("<<route_queue_assignment[index]<<") ";
		}
		
	}
	cout<<endl;
	cout<<endl;
}

void flow::assign_route_and_queue(int *route, int *route_queue_assignment, link::queue_reservation_state *state, int route_length){
	this->set_route(route, route_length);
	this->set_state(state, route_length);
	this->set_route_queue_assignment(route_queue_assignment, route_length);

	for(int index = 0; index < route_length; index++){
		if (-1 != route[index]){
            if (link::OPEN == state[index]){
                for (int frame_index = 0; frame_index < size; frame_index++){
			        link_list[route[index]]->update_gcl(index+frame_index, route_queue_assignment[index], state[index]); 
                }

            }
            else if (link::WAITING == state[index])  {
			    link_list[route[index]]->update_gcl(index, route_queue_assignment[index], state[index]); 
            }
			else{
				std::cerr<<"Trying to do invalid reservation in GCL for the flow id: "<<this->get_flow_id()<<std::endl;
				std::cerr<<"Invalid queue state is: "<<state[index]<<std::endl;
			}
		}
	}
	this->set_is_scheduled(true);
}

void flow::remove_route_and_queue_assignment(){

	int* route = this->get_route();
	link::queue_reservation_state* state = this->get_state();
	int* route_queue_assignment = this->get_route_queue_assignment();

	for(int index = 0; index < this->get_route_length(); index++){
		if (-1 != route[index]){
			if (link::OPEN == state[index]){
				for (int frame_index = 0; frame_index < size; frame_index++){
					link_list[route[index]]->update_gcl(index+frame_index, route_queue_assignment[index], link::FREE); 
				}

			}
			else if (link::WAITING == state[index])  {
				link_list[route[index]]->update_gcl(index, route_queue_assignment[index], link::FREE); 
			}
			else{
				std::cerr<<"Trying to free invalid reservation in GCL for the flow id: "<<this->get_flow_id()<<std::endl;
			}
		}
		else{
			std::cerr<<"Invalid link id: "<<route[index]<<" in the route of the flow id: "<<this->get_flow_id()<<std::endl;
		}
	}

	this->delete_route();
	this->delete_state();
	this->delete_route_queue_assignment();
}
