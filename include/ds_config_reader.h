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
	public:
		int read_node_config();
		int read_flow_config();

		configuration(std::string input_file_name);

		void tokenize(std::string const &str, const char delim, std::vector<std::string> &out);
};
