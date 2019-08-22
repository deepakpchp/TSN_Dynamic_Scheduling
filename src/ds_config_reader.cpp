#include <fstream>
#include <algorithm>
#include <iostream>
#include <ds_config_reader.h>
#include <ds_node.h>


/***************************************************************************************************
TODO
class: 
Function Name: 

Description: 

Return:
***************************************************************************************************/
configuration::configuration(std::string input_file_name){
	this->num_of_nodes = -1;
    this->num_of_connection = 0;
	this->input_file_name = input_file_name;
}


std::string configuration::get_node_type(int index){
    return this->node_type[index];
}

int configuration::get_num_of_nodes(){
    return this->num_of_nodes;
}

int configuration::get_num_of_connection(){
    return this->num_of_connection;
}

int* configuration::get_connection(int index){
    return this->connection[index];
}

int* configuration::get_flow_info(int flow_index){
	if(flow_index > this->num_of_flows){
		std::cerr << "Trying to get the details of a flow that doesn't exist!!\n";
		return NULL;
	}
	return (this->flow_info[flow_index]);	
}

int configuration::get_num_of_flows(){
	return this->num_of_flows;
}

bool configuration::get_reservation_availability(int index){
	return this->reservation_availability[index];
}

int* configuration::get_assigned_time_slot(int index){
	return this->assigned_time_slot[index];
}

int* configuration::get_route(int index){
	return this->route[index];
}

int* configuration::get_route_queue_assignment(int index){
	return this->route_queue_assignment[index];
}

link::queue_reservation_state* configuration::get_queue_state(int index){
	return this->queue_state[index];
}

int configuration::get_reservation_length(int index){
	return this->reservation_length[index];
}

/***************************************************************************************************
TODO
class: 
Function Name: 

Description: 

Return:
***************************************************************************************************/
int configuration::reset_reserv_config(int flow_index){
	if(flow_index > this->num_of_flows){
		std::cerr << "Trying to reset the reservation of a flow that doesn't exist!!\n";
		return (-1);
	}

	this->reservation_availability[flow_index] = false;
	this->reservation_length[flow_index] = 0;

	delete(this->assigned_time_slot[flow_index]);
	delete(this->route[flow_index]);
	delete(this->route_queue_assignment[flow_index]);
	delete(this->queue_state[flow_index]);
	return 0;
}

/***************************************************************************************************
TODO
class: 
Function Name: 

Description: 

Return:
***************************************************************************************************/
int configuration::read_flow_config(){
		std::ifstream inFile("../flows.txt");
		if(inFile.is_open()){
			this->num_of_flows = std::count(std::istreambuf_iterator<char>(inFile),
					std::istreambuf_iterator<char>(), '\n');
			inFile.close();
		}
		else {
			std::cerr << "Couldn't open config file for reading.\n";
		}
		std::ifstream cFile ("../flows.txt");
		if (0 < this->num_of_flows)
		{
			this->reservation_availability= new bool[this->num_of_flows];
			this->reservation_length = new int[this->num_of_flows];

			this->flow_info = new int*[this->num_of_flows];
			this->assigned_time_slot = new int*[this->num_of_flows];
			this->route = new int*[this->num_of_flows];
			this->route_queue_assignment = new int*[this->num_of_flows];
			this->queue_state = new link::queue_reservation_state*[this->num_of_flows];


			int flow_index = 0;
			std::string line;
			while(getline(cFile, line)){
				line.erase(std::remove_if(line.begin(), line.end(), isspace),
						line.end());
				if(line[0] == '#' || line.empty()){
					this->num_of_flows--;
					continue;
				}
				const char delim = ',';
				std::vector<std::string> tokens;
				auto flow = line.substr(1, line.size()-2);
				tokenize(flow, delim, tokens);
				if(tokens.size() < 6){
					std::cerr << "Flow not configured properly!!\nIgnoring the flow\n";
					continue;
				}
				int * ptr = new int[5];
				this->flow_info[flow_index] = ptr;
				for (unsigned int token_index = 0; token_index < 5; token_index++){
					this->flow_info[flow_index][token_index] = stoi(tokens[token_index]); 
				}

				if("True" == tokens[5]){
					//if((tokens.size()-6)%stoi(tokens[4]) != 0)
					if(0){
						std::cerr << "Flow reservation not configured properly for the flow :"<<flow_index<<std::endl;
						std::cerr<<"Tokens size :"<<tokens.size();
						this->reservation_availability[flow_index] = false;
						flow_index++;
						continue;
					}
					this->reservation_availability[flow_index] = true;
					this->reservation_length[flow_index] = tokens.size() - 6;

					this->assigned_time_slot[flow_index] = new int[this->reservation_length[flow_index]];
					this->route[flow_index] = new int[this->reservation_length[flow_index]];
					this->route_queue_assignment[flow_index] = new int[this->reservation_length[flow_index]];
					this->queue_state[flow_index] = new link::queue_reservation_state[this->reservation_length[flow_index]];

					for (unsigned int token_index = 0; token_index <  tokens.size() - 6; token_index++){
						std::vector<std::string> reservation_details;
						const char delim_2 = ':';
						tokenize(tokens[token_index+6], delim_2, reservation_details);
						if(4 != reservation_details.size()){
							std::cerr << "Reservation in flows not configured properly!!\nIgnoring the reservation\n";
							if(-1 == this->reset_reserv_config(flow_index)){
								std::cerr<<"Critical Error. Exiting Program\n";
								exit(0);
							}
							continue;
						}

						this->assigned_time_slot[flow_index][token_index] = stoi(reservation_details[0]);
						this->route[flow_index][token_index] = stoi(reservation_details[1]);
						this->route_queue_assignment[flow_index][token_index] = stoi(reservation_details[2]);
						if("OPEN" == reservation_details[3]){
							this->queue_state[flow_index][token_index] = link::OPEN;
						}
						else if("WAITING" == reservation_details[3]){
							this->queue_state[flow_index][token_index] = link::WAITING;
						}
						else{
							std::cerr << "Queue state in flows not configured properly!!\nIgnoring the reservation\n";
							if(-1 == this->reset_reserv_config(flow_index)){
								std::cerr<<"Critical Error. Exiting Program\n";
								exit(0);
							}
							continue;
						}
					}
				}
				flow_index++;	
			}

		}
		else {
			std::cerr << "Flow configuration file is empty!!!\nPlease configure the flows and try again \n";
		}
	return 0;
}

/***************************************************************************************************
TODO
class: 
Function Name: 

Description: 

Return:
***************************************************************************************************/
int configuration::read_node_config(){
		std::ifstream cFile ("../input.txt");
		if (cFile.is_open())
		{
			std::string line;
			const char delim = ',';
			while(getline(cFile, line)){
				line.erase(std::remove_if(line.begin(), line.end(), isspace),
						line.end());
				if(line[0] == '#' || line.empty())
					continue;
				auto delimiterPos = line.find("=");
				auto name = line.substr(0, delimiterPos);
				auto value = line.substr(delimiterPos + 1);
				if(name == "num_of_nodes"){
					this->num_of_nodes = stoi(value);
					this->node_type = new std::string[this->num_of_nodes];
					this->connection = new int*[this->num_of_nodes];
					for (int index = 0; index < this->num_of_nodes; index++){
						this->connection[index] = new int[2];
					}
				} 
                else if(name == "node_type"){
					if (0 > this->num_of_nodes){
						std::cerr<<"num_of_nodes not configured properly. Please check the input.txt for the configuration"<<std::endl;
						return -1;
					}
					auto node_type = value.substr(1, value.size()-2);
					std::vector<std::string> splited_node_type;
					tokenize(node_type, delim, splited_node_type);
					int index = 0;
					for (auto &s: splited_node_type) {
						this->node_type[index++] = s;
					}

				}
                else if(name == "connection"){
					if (0 > this->num_of_nodes){
						std::cerr<<"num_of_nodes not configured properly. Please check the input.txt for the configuration"<<std::endl;
						return -1;
					}

					auto connection = value.substr(1, value.size()-2);
					std::vector<std::string> conn;
					tokenize(connection, delim, conn);
			        const char delim_t = '-';
					for (auto &s: conn) {
					    std::vector<std::string> conn_t;
					    tokenize(s, delim_t, conn_t);
                        this->connection[this->num_of_connection][0] = stoi(conn_t[0]);
                        this->connection[this->num_of_connection][1] = stoi(conn_t[1]);
                        this->num_of_connection++;
					}
                }

			}

		}
		else {
			std::cerr << "Couldn't open config file for reading.\n";
		}
	return 0;
}

/***************************************************************************************************
TODO
class: 
Function Name: 

Description: 

Return:
***************************************************************************************************/
void configuration::tokenize(std::string const &str, const char delim,
			std::vector<std::string> &out)
{
	size_t start;
	size_t end = 0;

	while ((start = str.find_first_not_of(delim, end)) != std::string::npos)
	{
		end = str.find(delim, start);
		out.push_back(str.substr(start, end - start));
	}
}
