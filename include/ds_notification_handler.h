#ifndef DS_MR_HANDLER_H
#define DS_MR_HANDLER_H
#include <ds_utils.h>
class notification_handler{
    private:
       std::vector<int> flows_to_delete;
       std::vector<int> nodes_to_delete;
       std::vector<int> links_to_delete;

       std::vector<std::vector<int>> flows_to_modify;
       std::vector<std::vector<int>> flows_to_add;
       

    public:
       notification_handler();
       ~notification_handler();

       int read_modification_request();
	   void process_notification();
       void print();
};
#endif
