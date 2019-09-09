#include <ds_node.h>
#include <bitset>
#include <iostream>

extern link*** conn_link_matrix;
extern int** conn_matrix;
extern node** node_list;

link* link_list[MAX_NUM_LINKS];
int num_of_links =  0;
/***************************************************************************************************
TODO
class: 
Function Name: 

Description: 

Return:
***************************************************************************************************/
node::node(node_type type, int node_id){
	this->type = type;
	this->node_id = node_id;
	this->adj_node_count = 0;
	for(int index = 0; index < MAX_PORTS; index++){
		this->adj_node[index] = nullptr;
	}
}

/***************************************************************************************************
TODO
class: 
Function Name: 

Description: 

Return:
***************************************************************************************************/
node::~node(){
	std::cout<<"Destructor Called\n";
	unsigned adj_node_count = this->adj_node_count;
	for(unsigned int index = 0; index < adj_node_count; index++){
		/*Always index 0 is deleted because everytime adj node in index  0 is deleted, 
		  the adj node in the last index is copied to 0. By thisway the list remains contineous.
		  example deletion pattern:  0,1,2,3 -> 3,1,2 -> 2,1 -> 1 -> NULL*/
		this->disconnect(this->adj_node[0]->node_id);
	}
	node_list[this->get_node_id()] = nullptr;	
}

/***************************************************************************************************
TODO
class: 
Function Name: 

Description: 

Return:
***************************************************************************************************/
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
		conn_link_matrix[this->get_node_id()][adj_node->get_node_id()] = this->adj_link[this->adj_node_count];
		conn_matrix[this->get_node_id()][adj_node->get_node_id()] = 1;

		link_list[num_of_links++] = this->adj_link[this->adj_node_count];
		this->adj_node_count++;
		if(adj_node->adj_node_count < MAX_PORTS){
			adj_node->adj_node[adj_node->adj_node_count] = this;
			adj_node->adj_link[adj_node->adj_node_count] = new link(adj_node->node_id, this->node_id);
			conn_link_matrix [adj_node->get_node_id()][this->get_node_id()] = adj_node->adj_link[adj_node->adj_node_count];
			conn_matrix [adj_node->get_node_id()][this->get_node_id()] = 1;
			link_list[num_of_links++] = adj_node->adj_link[adj_node->adj_node_count];
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

/***************************************************************************************************
TODO
class: 
Function Name: 

Description: 

Return:
***************************************************************************************************/
void node::disconnect(int delete_node_id){
	for(unsigned int index = 0; index < this->adj_node_count; index++){
		if(this->adj_node[index]->node_id == delete_node_id){
			/*Delete the link to this node from the adjacent node in adj node obj*/
			node* adj_node = this->adj_node[index];
			bool deletion_flag = false;
			for(unsigned int index2 = 0; index2 < adj_node->adj_node_count; index2++){
				if(adj_node->adj_node[index2]->node_id == this->node_id){
					adj_node->adj_node[index2] = adj_node->adj_node[adj_node->adj_node_count-1];
					adj_node->adj_node[adj_node->adj_node_count-1] = nullptr;
  				
					//std::cout<<"Deleting link:"<<adj_node->adj_link[index2]->get_link_id()<<"\n";
					/*Link is copied and deleted later because the delete function for link will
					 call the destructor of the link class which will again try to delete the 
					 link and update the adjecent node using the dunction delete_one_side for 
					 the source node. This is done because sometimes when only the link is deleted
					 the corresponding src nodes adjecent node config has to be updated*/
  					link* link_to_delete = adj_node->adj_link[index2];
					adj_node->adj_link[index2] = adj_node->adj_link[adj_node->adj_node_count-1];
					adj_node->adj_link[adj_node->adj_node_count-1] = nullptr;


					adj_node->adj_node_count--;

					delete(link_to_delete);
					deletion_flag = true;
					break;
				}
			}
			if(false == deletion_flag){
				LOG("Link already delete from node id:"<<this->node_id 
						<<" to node_id:"<<delete_node_id);
			}

			/*Delete the adj_link to the adjacent node in this node*/
			this->adj_node[index] = this->adj_node[this->adj_node_count-1];
			this->adj_node[this->adj_node_count-1] = nullptr;

			//std::cout<<"Deleting link Out:"<<this->adj_link[index]->get_link_id()<<"\n";
			/*Link is copied and deleted later because the delete function for link will
			 call the destructor of the link class which will again try to delete the 
			 link and update the adjecent node using the dunction delete_one_side for 
			 the source node. This is done because sometimes when only the link is deleted
			 the corresponding src nodes adjecent node config has to be updated*/
			link* link_to_delete = this->adj_link[index];
			this->adj_link[index] = this->adj_link[this->adj_node_count-1];
			this->adj_link[this->adj_node_count-1] = nullptr;
			this->adj_node_count--;

			delete(link_to_delete);

			return;
		}
	}
	std::cout<<"Unnable to delete the link to node id :"<<delete_node_id<<std::endl;

}

/***************************************************************************************************
TODO
class: 
Function Name: 

Description: 

Return:
***************************************************************************************************/
void node::disconnect_one_side(int dst_node_id){
	for(unsigned int index = 0; index < this->adj_node_count; index++){
		if(this->adj_node[index]->node_id == dst_node_id){

			/*Delete the adj_link to the adjacent node in this node*/
			this->adj_node[index] = this->adj_node[this->adj_node_count-1];
			this->adj_node[this->adj_node_count-1] = nullptr;
			this->adj_link[index] = this->adj_link[this->adj_node_count-1];
			this->adj_link[this->adj_node_count-1] = nullptr;


			this->adj_node_count--;
			return;
		}
	}
	//std::cout<<"Either link already deleted or doesnt exist Node id:"<<dst_node_id<<std::endl;

}
/***************************************************************************************************
TODO
class: 
Function Name: 

Description: 

Return:
***************************************************************************************************/
void node::print(){
	for(unsigned int index = 0; index < this->adj_node_count; index++){
		std::cout<<this->adj_link[index]->get_link_id()<<"\t";
		std::cout<<this->adj_link[index]->get_passing_flow_count()<<"\t";
		std::cout<<this->node_id<<" -> "<<this->adj_node[index]->node_id<<"\t";
		int** gcl = this->adj_link[index]->get_gcl();
		for (int index2 = 0; index2 < HYPER_PERIOD; index2++){
			for(int index3 = QUEUES_PER_PORT-1; index3 >= 0; index3--){
                if (link::FREE == gcl[index2][index3]){
				    std::cout<<"-";
                } 
                else{
				    std::cout<<gcl[index2][index3];
                }
			}
			std::cout<<" ";
		}
		std::cout<<std::endl;
	}
}

/***************************************************************************************************
TODO
class: 
Function Name: 

Description: 

Return:
***************************************************************************************************/
int node::get_node_id(){
	return this->node_id;
}


/***************************************************************************************************
class: node 
Function Name: get_adj_node_count 

Description: Returns the number of adjecent nodes of the calling node object

Return: 0 if no adjecent nodes else number of adj of nodes
***************************************************************************************************/
int node::get_adj_node_count(){
	return this->adj_node_count;
}

#if 0
/***************************************************************************************************
TODO
class: 
Function Name: 

Description: 

Return:
***************************************************************************************************/
int node::get_flows_through_node(int **passing_flow_ids){

	int flow_count = 0;
G
	/*Check all the adj links to get all the flow ids passing through this node */
	for (int adj_index = 0; adj_index < MAX_PORTS; adj_index++){
		link* adj_link = this->adj_link[adj_index];
		int* flow_ids = NULL;
		int num_of_flows = 0;

		if (adj_link == NULL){
			continue;
		}
		/*get all the flow ids passing through adj link*/
		num_of_flows = adj_link->get_passing_flow_ids(&flow_ids);

		for (int index = 0; index < num_of_flows; index++){
			/*Add the flow_id without duplicate to the passing_flow_ids linked list*/
			int ret_val = passing_flow_ids->add_no_duplicate(flow_ids[index]);
			if(SUCCESS == ret_val){
				flow_count++;
			}
		}
		delete(flow_ids);
	}

	return flow_count;

}
#endif

/***************************************************************************************************
class: node
Function Name: add_passing_flow_to_list

Description: this function will add the flow id to the list of passing flows though calling node obj

Return: None
***************************************************************************************************/
void node::add_passing_flow_to_list(int flow_id){
	/*Add the flow to the list of passing flows on the link only if it doesnt already exist*/
	this->passing_flow_list.add_no_duplicate(flow_id);
}

/***************************************************************************************************
class: node
Function Name: delete_passing_flow_from_list

Description: this function will delete the flow id from the list of passing flows though 
			 calling node obj

Return: 0 - Successful, 1 Failure
***************************************************************************************************/
int node::delete_passing_flow_from_list(int flow_id){
	return this->passing_flow_list.remove(flow_id);
}


/***************************************************************************************************
class: node
Function Name: get_passing_flow_ids

Description: Returns all the flow ids passing through this node in a array passed

Return: 0 - Successful, 1 Failure
***************************************************************************************************/
int node::get_passing_flow_ids(int **passing_flow_ids){
	return this->passing_flow_list.get_all_data(passing_flow_ids);
}
