#include <ds_node.h>
#include <ds_link.h>
#include <ds_flow.h>
#include <string>
#include <vector>


class configuration{
	private:
		int num_of_nodes;
		node* node_list;

		int num_of_flows;
		flow* flow_list;
		std::string input_file_name;
    
        std::string node_type[10];
		
        int num_of_connection;
        int connection[10][2];

	public:
		int read_node_config();
		int read_flow_config();

        std::string get_node_type(int index);
		configuration(std::string input_file_name);

		void tokenize(std::string const &str, const char delim, std::vector<std::string> &out);
        int get_num_of_nodes();
        int get_num_of_connection();
        int* get_connection(int index);
};
