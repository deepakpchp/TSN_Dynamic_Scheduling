#include <iostream>
#include <ds_utils.h>
#include <ds_flow.h>

extern egress_link* link_list[];
extern flow** flow_list;

using namespace std;

/***************************************************************************************************
class: flow 
Function Name: constructor for flow

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
	this->reservation_status = flow::NEW;
}

/***************************************************************************************************
class: 
Function Name:  

Description: 

Return:
***************************************************************************************************/
flow::~flow(){
	if (flow::DELETE_FLOW != this->get_reservation_status()){
		FATAL("Trying to delete Flow id:"<<this->get_flow_id()<<" which is not ment to be deleted");
	}
	bool delete_status_flag = false;
	for (int index = 0; index < MAX_NUM_FLOWS; index++){
		if (nullptr == flow_list[index]){
			continue;
		}

		if (this->get_flow_id() == flow_list[index]->get_flow_id()){
			flow_list[index]->remove_route_and_queue_assignment(flow::DELETE_FLOW);
			flow_list[index] = nullptr;
			delete_status_flag = true;
			break;
		}
	}

	if (false == delete_status_flag){
		FATAL("Trying to remove Flow id:"<<this->get_flow_id()
				<<" from flow_list but it doesnt exist." );
	}

}


void flow::set_reservation_status(flow::reservation_state current_state){
	this->reservation_status = current_state;
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

void flow::set_state(egress_link::queue_reservation_state *state, int reservation_length){
	this->reservation_length = reservation_length;
	this->state = new egress_link::queue_reservation_state[this->reservation_length];
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

flow::reservation_state flow::get_reservation_status(){
	return this->reservation_status;
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

egress_link::queue_reservation_state* flow::get_state(){
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
void flow::delete_route_queue_assignment(flow::reservation_state new_state){
	this->reservation_length = 0;
	delete(this->route_queue_assignment);
	this->set_reservation_status(new_state);
	this->route_queue_assignment = NULL;
}


/***************************************************************************************************
class: flow
Function Name: print

Description: This Function will print the details of the calling flow object.

Return: void
***************************************************************************************************/
void flow::print(){

	const char* reservation_state_names[] =
	{
		stringify(DELETE_FLOW),
		stringify(NEW),
		stringify(MODIFIED),
		stringify(NODE_DELETED),
		stringify(LINK_DELETED),
		stringify(SCHEDULED)
	};

	cout<<"Flow_id: "<<this->get_flow_id()<<endl;
	cout<<"From node: "<<this->src_node_id<<" to "<<this->dst_node_id<<endl;
	cout<<"Deadline: "<<this->get_deadline()<<", Size: "<<this->get_size();
	cout<<", Period: "<<this->get_period() <<endl;
	cout<<"Scheduled_Status: "<<reservation_state_names[this->get_reservation_status()];

	if(SCHEDULED == this->get_reservation_status()){
		int reservation_length = this->get_reservation_length();
		int* route = this->get_route();
		int* route_queue_assignment = this->get_route_queue_assignment();
		int* assigned_time_slot = this->get_assigned_time_slot();
		egress_link::queue_reservation_state* state = this->get_state();
		cout<<"\nReservation Length: "<<reservation_length<<endl<<"Route: ";

		for (int index = 0; index < this->reservation_length; index ++){
			if(egress_link::WAITING == state[index]){
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
			 update_gcl functions of the egress_links along the route to update the Gate control list and
			 queue assignments of corresponding egress_links.

Return: None 
***************************************************************************************************/
void flow::assign_route_and_queue(int* assigned_time_slot, int* route, int* route_queue_assignment,
		egress_link::queue_reservation_state* state, int reservation_length){

	/*Update the flow object with the reseration details*/
	this->set_assigned_time_slot(assigned_time_slot, reservation_length);
	this->set_route(route, reservation_length);
	this->set_state(state, reservation_length);
	this->set_route_queue_assignment(route_queue_assignment, reservation_length);

	/*Update the gate control list on all the egress_links along the route*/
	for(int index = 0; index < reservation_length; index++){
		if ((-1 != route[index]) && (assigned_time_slot[index] < HYPER_PERIOD) 
				&& (assigned_time_slot[index] >= 0)){

            if (egress_link::OPEN == state[index] || egress_link::WAITING == state[index]){
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
	this->set_reservation_status(SCHEDULED);
}

/***************************************************************************************************
class: flow 
Function Name: remove_route_and_queue_assignment

Description: This function will delete the reservation on all the egress_egress_links along the route.

Return: None
***************************************************************************************************/
void flow::remove_route_and_queue_assignment(flow::reservation_state new_state){

	if (SCHEDULED != this->get_reservation_status() && 
			DELETE_FLOW != this->get_reservation_status()){
		ERROR("Trying to delete the schedule of unscheduled flow id:"<<this->get_flow_id());
		return;
	}

	int* assigned_time_slot = this->get_assigned_time_slot();
	int* route = this->get_route();
	egress_link::queue_reservation_state* state = this->get_state();
	int* route_queue_assignment = this->get_route_queue_assignment();

	for(int index = 0; index < this->get_reservation_length(); index++){
		if ((-1 != route[index]) && (assigned_time_slot[index] < HYPER_PERIOD) 
				&& (assigned_time_slot[index] >= 0)){

			if (egress_link::OPEN == state[index] || egress_link::WAITING == state[index]){
				link_list[route[index]]->update_gcl(assigned_time_slot[index], 
					 route_queue_assignment[index], this->get_flow_id(), egress_link::FREE); 
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
	this->delete_route_queue_assignment(new_state);
}
