#include <iostream>
#include <ds_utils.h>


/***************************************************************************************************
  TODO
class: 
Function Name: 

Description: 

Return:
***************************************************************************************************/
void tokenize(std::string const &str, const char delim,
        std::vector<std::string> &out)
{
    size_t start;
    size_t end = 0;

    while ((start = str.find_first_not_of(delim, end)) != std::string::npos)
    {   
        end = str.find(delim, start);
        out.push_back(str.substr(start, end - start));
    }   
}

/***************************************************************************************************
  TODO
class: 
Function Name: 

Description: 

Return:
***************************************************************************************************/
linked_list::linked_list()
{
    head = NULL;
    tail = NULL;
    count = 0;
}

/***************************************************************************************************
  TODO
class: 
Function Name: 

Description: 

Return:
***************************************************************************************************/
void linked_list::add(int n)
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

/***************************************************************************************************
  TODO
class: 
Function Name: 

Description: 

Return:
***************************************************************************************************/
int linked_list::add_no_duplicate(int n)
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

/***************************************************************************************************
  TODO
class: 
Function Name: 

Description: 

Return:
***************************************************************************************************/
int linked_list::remove(int n){

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

/***************************************************************************************************
  TODO
class: 
Function Name: 

Description: 

Return:
***************************************************************************************************/
int linked_list::check_if_data_exist(int data){
    list_node* temp = head;
    while (NULL != temp){
        if (data == temp->data){
            return SUCCESS;
        }
        temp = temp->next;
    }
    return FAILURE;

}		

/***************************************************************************************************
  TODO
class: 
Function Name: 

Description: 

Return:
***************************************************************************************************/
int linked_list::get_count(){
    return count;
}

/***************************************************************************************************
  TODO
class: 
Function Name: 

Description: 

Return:
***************************************************************************************************/
int linked_list::get_all_data(int **data){
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

/***************************************************************************************************
  TODO
class: 
Function Name: 

Description: 

Return:
***************************************************************************************************/
void linked_list::delete_all(){
    list_node* tmp = head;
    while (NULL != tmp){
        head = head->next;
        delete(tmp);
        tmp = head;
    }
    tail = NULL;
}
