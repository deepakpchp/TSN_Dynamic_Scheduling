#include <ds_node.h>
#include <bitset>
#include <iostream>

node::node(node_type type, int node_id){
	this->type = type;
	this->node_id = node_id;
	this->adj_node_count = 0;
	for(int index = 0; index < MAX_PORTS; index++){
		this->adj_node[index] = nullptr;
	}
}

node::~node(){
	for(unsigned int index = 0; index < this->adj_node_count; index++){
		this->disconnect(this->adj_node[index]->node_id);
	}
}

void node::connect(node* adj_node){
	for(unsigned int index = 0; index < this->adj_node_count; index++){
		if (this->adj_node[index] == adj_node){
			std::cout<<"Already a connection exist to the node : "<<adj_node->node_id<<" from node : "<<this->node_id <<std::endl;
			return;
		}

	}

	for(unsigned int index = 0; index < this->adj_node_count; index++){
		if (this->adj_node[index]->node_id == adj_node->node_id){
			std::cout<<"Duplicate node id detected!!! Please review the code"<<std::endl;
			return;
		}
	}

	if(this->adj_node_count < MAX_PORTS){
		this->adj_node[this->adj_node_count] = adj_node;
		this->adj_link[this->adj_node_count] = new link(this->node_id, adj_node->node_id);
		this->adj_node_count++;
		if(adj_node->adj_node_count < MAX_PORTS){
			adj_node->adj_node[adj_node->adj_node_count] = this;
			adj_node->adj_link[adj_node->adj_node_count] = new link(adj_node->node_id, this->node_id);
			adj_node->adj_node_count++;
		}
		else{
			std::cout<<"Maximum number of connections reached in node :"<<adj_node->node_id<<std::endl;
		}
	}
	else{
		std::cout<<"Connection to node not possible"<<std::endl;
	}
}

void node::disconnect(int delete_node_id){
	for(unsigned int index = 0; index < this->adj_node_count; index++){
		if(this->adj_node[index]->node_id == delete_node_id){
			/*Delete the link to this node in the adjacent node*/
			node* adj_node = this->adj_node[index];
			bool deletion_flag = false;
			for(unsigned int index2 = 0; index2 < adj_node->adj_node_count; index2++){
				if(adj_node->adj_node[index2]->node_id == this->node_id){
					adj_node->adj_node[index2] = adj_node->adj_node[adj_node->adj_node_count-1];
					adj_node->adj_node[adj_node->adj_node_count-1] = nullptr;
					
					delete(adj_node->adj_link[index2]);
					adj_node->adj_link[index2] = adj_node->adj_link[adj_node->adj_node_count-1];
					adj_node->adj_link[adj_node->adj_node_count-1] = nullptr;


					adj_node->adj_node_count--;
					deletion_flag = true;
					break;
				}
			}
			if(false == deletion_flag){
				std::cout<<"Unnable to delete the link of node id:"<<this->node_id <<" in the node :"<<delete_node_id<<std::endl;
				break;
			}

			/*Delete the adj_link to the adjacent node in this node*/
			this->adj_node[index] = this->adj_node[this->adj_node_count-1];
			this->adj_node[this->adj_node_count-1] = nullptr;

			delete(this->adj_link[index]);
			this->adj_link[index] = this->adj_link[this->adj_node_count-1];
			this->adj_link[this->adj_node_count-1] = nullptr;


			this->adj_node_count--;
			return;
		}
	}
	std::cout<<"Unnable to delete the link to node id :"<<delete_node_id<<std::endl;

}

void node::print(){
	for(unsigned int index = 0; index < this->adj_node_count; index++){
		std::cout<<this->adj_link[index]->get_link_id()<<"  ";
		std::cout<<this->node_id<<" -> "<<this->adj_node[index]->node_id<<"  ";
		char* gcl = this->adj_link[index]->get_gcl();
		for (int index2 = 0; index2 < HYPER_PERIOD; index2++){
			std::bitset<8> x(gcl[index2]);
			std::cout<<x<<" ";
		}
		std::cout<<std::endl;
	}
	std::cout<<std::endl;
}