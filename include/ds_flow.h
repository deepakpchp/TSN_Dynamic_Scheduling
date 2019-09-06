#ifndef DS_FLOW_H
#define DS_FLOW_H
#include <ds_link.h>
#include <ds_utils.h>
#define MAX_NUM_FLOWS 50


class flow{
	private:
		int flow_id;
		//		bool is_scheduled;
		int src_node_id;
		int dst_node_id;
		int deadline;
		int size;
		int period;

		int* route;
		int* assigned_time_slot;
		link::queue_reservation_state* state;
		int* route_queue_assignment;
		int reservation_length;
	public:
		enum reservation_state{
			DELETE_FLOW,
			NEW,
			MODIFIED,
			NODE_DELETED,
			LINK_DELETED,
			SCHEDULED
		};
		reservation_state reservation_status;
		static int id_flow;
		flow(int src_node_id, int dst_node_id, int deadline, int length, int period);
		~flow();
		void set_reservation_status(flow::reservation_state current_state);
		void set_src_node_id(int src_node_id);
		void set_dst_node_id(int dst_node_id);
		void set_deadline(int deadline);
		void set_size(int length);
		void set_period(int period);
		void set_route(int *route, int reservation_length);
		void set_assigned_time_slot(int* assigned_time_slot, int reservation_length);
		void set_state(link::queue_reservation_state *state, int reservation_length);
		void set_route_queue_assignment(int* route_queue_assignment, int reservation_length);

		int get_flow_id();
		flow::reservation_state get_reservation_status();
		int get_src_node_id();
		int get_dst_node_id();
		int get_deadline();
		int get_size();
		int get_period();
		int* get_route();
		int* get_assigned_time_slot();
		int* get_route_queue_assignment();
		link::queue_reservation_state* get_state();
		int get_reservation_length();

		void delete_assigned_time_slot();
		void delete_route();
		void delete_route_queue_assignment(flow::reservation_state new_state);
		void delete_state();


		void print();
		void assign_route_and_queue(int* assigned_time_slot, int* route, int* route_queue_assignment, link::queue_reservation_state state[], int reservation_length);
		void remove_route_and_queue_assignment(flow::reservation_state new_state);
};
#endif
