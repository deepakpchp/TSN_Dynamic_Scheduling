#ifndef DS_EGRESS_LINK_H
#define DS_EGRESS_LINK_H
#include <ds_utils.h>

#define HYPER_PERIOD 8
#define QUEUES_PER_PORT 8
#define GCL_INIT 0x1F
#define NUM_OF_QUEUES_FOR_TT 3

class egress_link{
	private:
		int link_id;
		int src_node_id;
		int dst_node_id;
		int** gcl;
		int open_slots_count;
		int wait_slots_count;
		bool slot_transmission_availablity[HYPER_PERIOD]; 
		linked_list passing_flow_list;

	public:
		enum queue_reservation_state{
			WAITING,
			OPEN,
			FREE
		};
		static int id_link;
		egress_link(int, int);
		~egress_link();

		void set_link_id(int link_id);
		void set_src_node_id(int src_node_id);
		void set_dst_node_id(int dst_node_id);
		void set_gcl(int* glc, int time_slot);
		void add_passing_flow_to_list(int flow_id);
		int delete_passing_flow_from_list(int flow_id);

		int get_link_id();
		int get_src_node_id();
		int get_dst_node_id();
		int get_open_slots_count();
		int get_wait_slots_count();
		int get_passing_flow_count();
		int** get_gcl();
		void update_gcl(int time_slot, int flow_id,	int route_queue_assignment, 
				queue_reservation_state state);
		int do_slot_allocation(int* flow_transmition_slot, int *reserved_queue_index, 
				int size, int period, int deadline);
		int do_slot_allocation_inverse(int* flow_transmition_slot, int *reserved_queue_index, 
				int size, int period, int deadline);
		int get_passing_flow_ids(int** flows);
};
#endif
