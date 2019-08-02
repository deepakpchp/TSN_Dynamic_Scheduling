#ifndef DS_FLOW_H
#define DS_FLOW_H


class flow{
	private:
		int flow_id;
		bool is_scheduled;
		int src_node_id;
		int dst_node_id;
		int deadline;
		int size;
		int* route;
		int* route_queue_assignment;
		int route_length;
	public:
		static int id_flow;
		flow(int src_node_id, int dst_node_id, int deadline, int length);
		void set_is_scheduled(bool is_scheduled);
		void set_src_node_id(int src_node_id);
		void set_dst_node_id(int dst_node_id);
		void set_deadline(int deadline);
		void set_size(int length);
		void set_route(int *route, int route_length);
		void set_route_queue_assignment(int *route_queue_assignment, int route_length);

		int get_flow_id();
		bool get_is_scheduled();
		int get_src_node_id();
		int get_dst_node_id();
		int get_deadline();
		int get_size();
		int* get_route();
		int* get_route_queue_assignment();
		int get_route_length();

		void print();
		void assign_route_and_queue(int *route, int *route_queue_assignment, int route_length);
};
#endif
