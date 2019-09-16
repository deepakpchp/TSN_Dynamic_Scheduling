#ifndef DS_SCHEDULER_H
#define DS_SCHEDULER_H

#include <unistd.h> 
#include <string.h>
#include <fstream>
#define _BSD_SOURCE
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <ds_config_reader.h>
#include <ds_node.h>
#include <ds_egress_link.h>
#include <ds_route.h>
#include <ds_notification_handler.h>
#include <ds_flow.h>
#include <ds_utils.h>

#define K_SHORTEST_PATH 3
#define FILE_READ_BUFFER_SUZE 100

int read_and_configure_nodes(configuration* config);
int read_and_configure_flows(configuration* config);
int get_link_id(int src_node_id, int dst_node_id);
int schedule_flow(int flow_index);
int perform_flow_reservation(flow* flow, int* route, int route_length);
int perform_flow_reservation_inverse(flow* flow, int* route, int route_length);
void gen_tmp_conn_matrix(int** tmp_conn_matrix, flow* flow_to_scheduled);
void print_conn_link_matrix_details(configuration* config_p);
int get_k_shortest_paths(int** conn_matrix, int src_id, int dst_id, int*** route, 
		int route_length[]);
egress_link*  get_link_to_detele_in_cm(int* route, int route_length, egress_link* link_not_to_delete);
void rank_flows(int** route, int* route_length, int num_of_paths);
int delete_node(int node_id);
flow* get_flow_ptr_from_id(int flow_id);
int delete_flow(int flow_index);
int dynamic_scheduling();
int process_notification();
void cleanup();
using namespace std;

#endif
