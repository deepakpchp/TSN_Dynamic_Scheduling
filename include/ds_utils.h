#ifndef DS_UTILS_H
#define DS_UTILS_H
#include <vector>
#include <string>
#include <iostream>

#define SUCCESS 0
#define FAILURE 1

#define INFO(X) std::cout<<"Info: "<<X<<std::endl
//#define LOG(X) std::cout<<"Log! "<<__FILE__<<":"<<__LINE__<<" "<<X<<std::endl
#define LOG(X) 
#define ERROR(X) std::cerr<<"Error!! "<<__FILE__<<":"<<__LINE__<<" "<<X<<std::endl
#define FATAL(X) std::cerr<<"Fatal Error!!! "<<__FILE__<<":"<<__LINE__<<" "<<X\
					<<std::endl<<std::endl; exit(0)

#define stringify( name ) # name

void tokenize(std::string const &str, const char delim,
            std::vector<std::string> &out);

struct list_node
{
    int data;
    list_node *next;
};

class linked_list {
	private:
		int count;
		list_node *head, *tail;
	public:
        linked_list();
        void add(int n);
        int add_no_duplicate(int n);
        int remove(int n);
        int check_if_data_exist(int data);
        int get_count();
        int get_all_data(int **data);
        void delete_all();
};

#endif
