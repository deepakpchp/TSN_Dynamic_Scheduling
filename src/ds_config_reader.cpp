#include <fstream>
#include <algorithm>
#include <iostream>
#include <ds_config_reader.h>

int configuration::read_node_config(){
		std::ifstream cFile ("../input.txt");
		if (cFile.is_open())
		{
			std::string line;
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
				} else if(name == "node_type"){
					if (0 > this->num_of_nodes){
						std::cerr<<"num_of_nodes not configured properly. Please check the input.txt for the configuration"<<std::endl;
						return -1;
					}
					std::string line[10];
//					auto delimiterPos = value.find(
					auto node_type = value.substr(1, value.size()-2);
					std::cout<<node_type;
					const char delim = ',';
					std::vector<std::string> out;
					tokenize(node_type, delim, out);
					int index = 0;
					for (auto &s: out) {
						line[index] = s;
						std::cout << line[index++] << '\n';
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


int configuration::read_flow_config(){
		flow* flow_list[3];
		std::ifstream cFile ("../flows.txt");
		if (cFile.is_open())
		{
			std::string line;
			while(getline(cFile, line)){
				line.erase(std::remove_if(line.begin(), line.end(), isspace),
						line.end());
				if(line[0] == '#' || line.empty())
					continue;
				const char delim = ',';
				std::vector<std::string> out;
				auto flow = line.substr(1, line.size()-2);
				tokenize(flow, delim, out);
				std::cout<<"Out is :"<<out[0];
			}

		}
		else {
			std::cerr << "Couldn't open config file for reading.\n";
		}
	return 0;
}
