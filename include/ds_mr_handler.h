#ifndef DS_MR_HANDLER_H
#define DS_MR_HANDLER_H
#include <ds_utils.h>
class mr_handler{
    private:
       std::vector<int> flows_to_delete;
       std::vector<int> flows_to_add;

       std::vector<int> flows_to_modify;
       std::vector<int> nodes_to_delete;
       std::vector<int> links_to_delete;
       

    public:
       mr_handler();
       ~mr_handler();

       int read_modification_request();
       void print();
};
#endif
