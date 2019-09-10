#include <ds_node.h>
#include <ds_egress_link.h>
#include <ds_flow.h>
#include <string>
#include <vector>


class configuration{
	private:
		std::string input_file_name;

		int num_of_nodes;
		std::string* node_type;
		int** connection;
		int num_of_connection;

		int num_of_flows;
		int** flow_info;
		bool* reservation_availability;

		int** assigned_time_slot;
		int** route;
		int** route_queue_assignment;
		egress_link::queue_reservation_state** queue_state;
		int* reservation_length;


	public:
		int read_node_config();
		int read_flow_config();

		std::string get_node_type(int index);
		configuration(std::string input_file_name);

		void tokenize(std::string const &str, const char delim, std::vector<std::string> &out);
		int get_num_of_nodes();
		int get_num_of_connection();
		int reset_reserv_config(int flow_index);
		int* get_connection(int index);

		int* get_flow_info(int flow_index);
		int get_num_of_flows();		
		bool get_reservation_availability(int index);
		int* get_assigned_time_slot(int index);
		int* get_route(int index);
		int* get_route_queue_assignment(int index);
		egress_link::queue_reservation_state* get_queue_state(int index);
		int get_reservation_length(int index);

};
