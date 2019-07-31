#ifndef DS_LINK_H
#define DS_LINK_H

#define HYPER_PERIOD 8
#define GCL_INIT 0x1F

class link{
	private:
		int link_id;
		int src_node_id;
		int dst_node_id;
		char gcl[HYPER_PERIOD];
	public:
		static int id;
		link(int, int);
		void set_link_id(int link_id);
		int get_link_id();
		void set_src_node_id(int src_node_id);
		int get_src_node_id();
		void set_dst_node_id(int dst_node_id);
		int get_dst_node_id();
		void set_gcl(char *glc);
		char* get_gcl();
};
#endif
