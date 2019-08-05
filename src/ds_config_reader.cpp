#include <ds_node.h>
#include <ds_link.h>
#include <ds_flow.h>

class configuration{
	private:
		int num_of_nodes;
		node* node_list;

		int num_of_flows;
		flow* flow_list;
	public:
		void read_configuration(std::string input);
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
			}

		}
		else {
			std::cerr << "Couldn't open config file for reading.\n";
		}

}
