#ifndef DS_FLOW_H
#define DS_FLOW_H
#include <ds_link.h>

class flow{
	private:
		int flow_id;
		bool is_scheduled;
		int src_node_id;
		int dst_node_id;
		int deadline;
		int size;
		int period;

		int* route;
		link::queue_reservation_state* state;
		int* route_queue_assignment;
		int route_length;
	public:
		static int id_flow;
		flow(int src_node_id, int dst_node_id, int deadline, int length, int period);
		void set_is_scheduled(bool is_scheduled);
		void set_src_node_id(int src_node_id);
		void set_dst_node_id(int dst_node_id);
		void set_deadline(int deadline);
		void set_size(int length);
		void set_period(int period);
		void set_route(int *route, int route_length);
		void set_state(link::queue_reservation_state *state, int route_length);
		void set_route_queue_assignment(int *route_queue_assignment, int route_length);

		int get_flow_id();
		bool get_is_scheduled();
		int get_src_node_id();
		int get_dst_node_id();
		int get_deadline();
		int get_size();
		int get_period();
		int* get_route();
		int* get_route_queue_assignment();
		link::queue_reservation_state* get_state();
		int get_route_length();

		void delete_route();
		void delete_route_queue_assignment();
		void delete_state();


		void print();
		void assign_route_and_queue(int *route, int *route_queue_assignment, link::queue_reservation_state state[], int route_length);
		void remove_route_and_queue_assignment();
};
#endif
