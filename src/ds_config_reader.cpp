#include <fstream>
#include <algorithm>
#include <iostream>
#include <ds_config_reader.h>
#include <ds_node.h>

int configuration::read_configuration(){
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
				std::cout << name << " " << value << '\n';
				if(name == "num_of_nodes"){
					this->num_of_nodes = stoi(value);
					std::cout<<"kehooo "<<value<<std::endl;
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
					//	std::cout << s << '\n';
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


configuration::configuration(std::string input_file_name){
	this->num_of_nodes = -1;
    this->num_of_connection = 0;
	this->input_file_name = input_file_name;
}

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
