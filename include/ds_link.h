#ifndef DS_LINK_H
#define DS_LINK_H

#define HYPER_PERIOD 8
#define QUEUES_PER_PORT 8
#define GCL_INIT 0x1F
#define NUM_OF_QUEUES_FOR_TT 3

class link{
	private:
		int link_id;
		int src_node_id;
		int dst_node_id;
		int** gcl;
		int open_slots_count;
		int waiting_slots_count;
		bool slot_transmission_availablity[HYPER_PERIOD]; 
	public:
		enum queue_reservation_state{
			WAITING,
			OPEN,
			FREE
		};
		static int id_link;
		link(int, int);

		void set_link_id(int link_id);
		void set_src_node_id(int src_node_id);
		void set_dst_node_id(int dst_node_id);
		void set_gcl(int* glc, int time_slot);

		int get_link_id();
		int get_src_node_id();
		int get_dst_node_id();
		int get_open_slots_count();
		int get_waiting_slots_count();
		int** get_gcl();
		void update_gcl(int time_slot, int route_queue_assignment, queue_reservation_state state);
		int do_slot_allocation(int* flow_transmition_slot, int *reserved_queue_index, 
				int size, int period, int deadline);
};
#endif
