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

void link::set_gcl(char *gcl){
	for(int index = 0; index < HYPER_PERIOD; index++){
		this->gcl[index] = gcl[index];
	}
}
char* link::get_gcl(){
	return this->gcl;
}

link::link(int src_node_id, int dst_node_id){
	this->link_id = this->id++;
	this->src_node_id = src_node_id;
	this->dst_node_id = dst_node_id;
	for (int index = 0; index < HYPER_PERIOD; index++){
		this->gcl[index] = GCL_INIT;
	}
}
