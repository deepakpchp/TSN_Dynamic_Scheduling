#include <iostream>
#include <ds_utils.h>
#include <ds_flow.h>

extern link* link_list[];
using namespace std;

/***************************************************************************************************
class: flow 
Function Name: flow(int src_node_id, int dst_node_id, int deadline, int size, int period)

Description: Initializer for flow class object

Return: Void
***************************************************************************************************/
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
	this->reservation_length = 0;
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

void flow::set_assigned_time_slot(int *assigned_time_slot, int reservation_length){
	this->reservation_length = reservation_length;
	this->assigned_time_slot = new int[this->reservation_length];
	for (int index = 0; index < reservation_length; index++){
		this->assigned_time_slot[index] = assigned_time_slot[index];
	}
}

void flow::set_route(int *route, int reservation_length){
	this->reservation_length = reservation_length;
	this->route = new int[this->reservation_length];
	for (int index = 0; index < reservation_length; index++){
		this->route[index] = route[index];
	}
}

void flow::set_state(link::queue_reservation_state *state, int reservation_length){
	this->reservation_length = reservation_length;
	this->state = new link::queue_reservation_state[this->reservation_length];
	for (int index = 0; index < reservation_length; index++){
		this->state[index] = state[index];
	}
}



void flow::set_route_queue_assignment(int *route_queue_assignment, int reservation_length){
	this->reservation_length = reservation_length;
	this->route_queue_assignment = new int[this->reservation_length];
	for (int index = 0; index < reservation_length; index++){
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

int flow::get_period(){
	return this->period;
}

int* flow::get_assigned_time_slot(){
	return this->assigned_time_slot;
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

int flow::get_reservation_length(){
	return this->reservation_length;
}

/***************************************************************************************************
TODO
class: 
Function Name: 

Description: 

Return:
***************************************************************************************************/
void flow::delete_assigned_time_slot(){
	this->reservation_length = 0;
	delete(this->assigned_time_slot);
	this->assigned_time_slot = NULL;
}

/***************************************************************************************************
TODO
class: 
Function Name: 

Description: 

Return:
***************************************************************************************************/
void flow::delete_route(){
	this->reservation_length = 0;
	delete(this->route);
	this->route = NULL;
}

/***************************************************************************************************
TODO
class: 
Function Name: 

Description: 

Return:
***************************************************************************************************/
void flow::delete_state(){
	this->reservation_length = 0;
	delete(this->state);
	this->state = NULL;
}

/***************************************************************************************************
TODO
class: 
Function Name: 

Description: 

Return:
***************************************************************************************************/
void flow::delete_route_queue_assignment(){
	this->reservation_length = 0;
	delete(this->route_queue_assignment);
	this->set_is_scheduled(false);
	this->route_queue_assignment = NULL;
}


/***************************************************************************************************
class: flow
Function Name: print

Description: This Function will print the details of the calling flow object.

Return: void
***************************************************************************************************/
void flow::print(){
	cout<<"Flow_id: "<<this->get_flow_id()<<endl;
	cout<<"From node: "<<this->src_node_id<<" to "<<this->dst_node_id<<endl;
	cout<<"Deadline: "<<this->get_deadline()<<", Size: "<<this->get_size();
	cout<<", Period: "<<this->get_period() <<endl;
    cout<<"Scheduled_Status: "<<this->get_is_scheduled();

	if(this->get_is_scheduled()){
		int reservation_length = this->get_reservation_length();
		int* route = this->get_route();
		int* route_queue_assignment = this->get_route_queue_assignment();
		int* assigned_time_slot = this->get_assigned_time_slot();
		link::queue_reservation_state* state = this->get_state();
		cout<<"\nReservation Length: "<<reservation_length<<endl<<"Route: ";

		for (int index = 0; index < this->reservation_length; index ++){
			if(link::WAITING == state[index]){
				cout<<"W";
			}
			else{
				cout<<"T";
			}
			cout<<"@"<<assigned_time_slot[index]<<":on"<<route[index]
				<<":q("<<route_queue_assignment[index]<<") ";
		}
	}
	cout<<endl<<endl;
}

/***************************************************************************************************
class: flow
Function Name: assign_route_and_queue 
			   
Description: This function will assigne the time slot assignment, route details and 
			 queue assignemt for the flow along the route for the flow. This will also call the
			 update_gcl functions of the links along the route to update the Gate control list and
			 queue assignments of corresponding links.

Return: None 
***************************************************************************************************/
void flow::assign_route_and_queue(int* assigned_time_slot, int* route, int* route_queue_assignment,
		link::queue_reservation_state* state, int reservation_length){

	/*Update the flow object with the reseration details*/
	this->set_assigned_time_slot(assigned_time_slot, reservation_length);
	this->set_route(route, reservation_length);
	this->set_state(state, reservation_length);
	this->set_route_queue_assignment(route_queue_assignment, reservation_length);

	/*Update the gate control list on all the links along the route*/
	for(int index = 0; index < reservation_length; index++){
		if ((-1 != route[index]) && (assigned_time_slot[index] < HYPER_PERIOD) 
				&& (assigned_time_slot[index] >= 0)){

            if (link::OPEN == state[index] || link::WAITING == state[index]){
			    link_list[route[index]]->update_gcl(assigned_time_slot[index], 
						route_queue_assignment[index],this->get_flow_id(), state[index]); 
            }
			else{
				ERROR("Trying to do invalid reservation in GCL for the flow id:"
						<<this->get_flow_id()<< " with queue state:"<<state[index]);
			}
		}
		else{
				ERROR("Invalid resrvation for the flow:"<<this->get_flow_id()
						<<". Time slot:"<<assigned_time_slot[index]<<" link_id:"<<route[index]);
			
		}
	}
	this->set_is_scheduled(true);
}

/***************************************************************************************************
class: flow 
Function Name: remove_route_and_queue_assignment

Description: This function will delete the reservation on all the links along the route.

Return: None
***************************************************************************************************/
void flow::remove_route_and_queue_assignment(){

	if (true != this->get_is_scheduled()){
		ERROR("Trying to delete the schedule of unscheduled flow id:"<<this->get_flow_id());
		return;
	}

	int* assigned_time_slot = this->get_assigned_time_slot();
	int* route = this->get_route();
	link::queue_reservation_state* state = this->get_state();
	int* route_queue_assignment = this->get_route_queue_assignment();

	for(int index = 0; index < this->get_reservation_length(); index++){
		if ((-1 != route[index]) && (assigned_time_slot[index] < HYPER_PERIOD) 
				&& (assigned_time_slot[index] >= 0)){

			if (link::OPEN == state[index] || link::WAITING == state[index]){
				link_list[route[index]]->update_gcl(assigned_time_slot[index], 
					 route_queue_assignment[index], this->get_flow_id(), link::FREE); 
			}
			else{
				std::cerr<<"Trying to free invalid reservation in GCL for the flow id: "
					<<this->get_flow_id()<<std::endl;
			}
		}
		else{
			std::cerr<<"Trying to delete invalid link id: "<<route[index]
				<<" in the route of the flow id: "<<this->get_flow_id()<<std::endl;
		}
	}

	this->delete_assigned_time_slot();
	this->delete_route();
	this->delete_state();
	this->delete_route_queue_assignment();
}
