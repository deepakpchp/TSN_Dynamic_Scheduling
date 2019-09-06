#include <ds_utils.h>
#include <ds_mr_handler.h>
#include <fstream>
#include <algorithm>
#include <vector>
#include <string>
#include <ds_link.h>

extern link*** conn_link_matrix;

/***************************************************************************************************
class: mr_handler
Function Name: constructor

Description:  

Return: None
***************************************************************************************************/
mr_handler::mr_handler(){
}

/***************************************************************************************************
class: mr_handler
Function Name: destructor

Description: cleanup all the data structurs 

Return: None
***************************************************************************************************/
mr_handler::~mr_handler(){
    flows_to_delete.clear();
    flows_to_add.clear();
    flows_to_modify.clear();
    nodes_to_delete.clear();
    links_to_delete.clear();
}

/***************************************************************************************************
//TODO
class: 
Function Name: 

Description:  

Return: None
***************************************************************************************************/
int mr_handler::read_modification_request(){
    std::ifstream inFile("../configuration/notification.txt");
    if(!inFile.is_open()){
        std::cerr << "Couldn't open notification file for reading.\n";
        return FAILURE;
    }    

//    std::ifstream inFile("../configuration/notification.txt");
    std::string line;
    const char delim = ',';
    while(getline(inFile, line)){
        line.erase(std::remove_if(line.begin(), line.end(), isspace),
                line.end());
        if(line[0] == '#' || line.empty())
            continue;
        auto delimiterPos = line.find("=");
        auto name = line.substr(0, delimiterPos);
        auto value = line.substr(delimiterPos + 1);

        if(name == "nodes_to_delete"){
            auto delete_node_list = value.substr(1, value.size()-2);
            std::vector<std::string> node_ids;
            tokenize(delete_node_list, delim, node_ids);
            for (auto &node_id: node_ids) {
                this->nodes_to_delete.push_back(stoi(node_id));
            }

        }
        else if(name == "flows_to_delete"){
            auto delete_flow_list = value.substr(1, value.size()-2);
            std::vector<std::string> flow_ids;
            tokenize(delete_flow_list, delim, flow_ids);
            for (auto &flow_id: flow_ids) {
                this->flows_to_delete.push_back(stoi(flow_id));
            }
        }
        else if(name == "links_to_delete"){
            auto delete_link_list = value.substr(1, value.size()-2);
            std::vector<std::string> connections;
            tokenize(delete_link_list, delim, connections);
            const char delim_t = '-';
            for (auto &connection: connections) {
                std::vector<std::string> conn_t;
                tokenize(connection, delim_t, conn_t);
                int src_node_id = stoi(conn_t[0]);
                int dst_node_id = stoi(conn_t[1]);

                link* link_ptr = conn_link_matrix[src_node_id][dst_node_id];
                if (nullptr == link_ptr){
                    ERROR("Trying to delete a link that doesnt exist, src_node_id:"<<src_node_id
                            <<" dst_node_id:"<<dst_node_id);
                    continue;
                }
                this->links_to_delete.push_back(link_ptr->get_link_id());
            }

        }
    }
    return SUCCESS;
}

/***************************************************************************************************
//TODO
class: 
Function Name: 

Description:  

Return: None
***************************************************************************************************/
void mr_handler::print(){
    std::cout<<"Flows to delete: ";
    for(auto &flow_id: flows_to_delete) {
        std::cout<<flow_id<<" ";
    }
    std::cout<<std::endl<<"Nodes to delete: ";
    for(auto &node_id: nodes_to_delete) {
        std::cout<<node_id<<" ";
    }
    std::cout<<std::endl<<"Links to delete: ";
    for(auto &link_id: links_to_delete) {
        std::cout<<link_id<<" ";
    }
    std::cout<<std::endl;
}
