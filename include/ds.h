#ifndef DS_H
#define DS_H

#include <iostream>
#define SUCCESS 0
#define FAILURE 1

#define INFO(X) std::cout<<"Info: "<<X<<std::endl
//#define LOG(X) std::cout<<"Log! "<<__FILE__<<":"<<__LINE__<<" "<<X<<std::endl
#define LOG(X) 
#define ERROR(X) std::cerr<<"Error!! "<<__FILE__<<":"<<__LINE__<<" "<<X<<std::endl
#define FATAL(X) std::cerr<<"Fatal Error!!! "<<__FILE__<<":"<<__LINE__<<" "<<X<<std::endl<<std::endl; exit(0)

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
		linked_list()
		{
			head = NULL;
			tail = NULL;
			count = 0;
		}

		void add(int n)
		{
			list_node *temp = new list_node;
			temp->data = n;
			temp->next = NULL;

			if(head == NULL)
			{
				head = temp;
				tail = temp;
			}
			else
			{
				tail->next = temp;
				tail = tail->next;
			}
			count++;
		}

		int add_no_duplicate(int n)
		{
			list_node* tmp = head;

			while(NULL != tmp){
				if (n == tmp->data){
					return FAILURE;
				}
				tmp = tmp->next;
			}

			list_node *temp = new list_node;
			temp->data = n;
			temp->next = NULL;

			if(head == NULL)
			{
				head = temp;
				tail = temp;
			}
			else
			{
				tail->next = temp;
				tail = tail->next;
			}
			count++;
			return SUCCESS;
		}

		int remove(int n){

			if (NULL == head){
				return FAILURE;
			}
			list_node *temp = head;
			list_node *pre = head;

			if (n == temp->data){
				delete(temp);
				head = temp->next;
				count--;
				if(count == 0){
					tail = NULL;
				}
				return SUCCESS;
			}


			temp = temp->next;
			while (temp != NULL){
				if (n == temp->data){
					pre->next = temp->next;
					delete(temp);
					count--;
					return SUCCESS;
				}
				temp = temp->next;
			}
			return FAILURE;
		}

		int check_if_data_exist(int data){
			list_node* temp = head;
			while (NULL != temp){
				if (data == temp->data){
					return SUCCESS;
				}
				temp = temp->next;
			}
			return FAILURE;

		}		

		int get_count(){
			return count;
		}

		int get_all_data(int **data){
			(*data)  = new int[count];
			list_node* tmp = head;
			int index = 0;
			while (tmp != NULL){
				(*data)[index++] = tmp->data;
				tmp = tmp->next;
			}
			if (count != index){
				FATAL("Number of nodes and count in the list doesn't match");
			}
			return index;
		}
};

#endif
