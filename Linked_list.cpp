#include <iostream>
using namespace std;

struct node{
    int data;
    node * next;
};
class linked_list{
    node * list = NULL;
    public:
    void insertion_at_start(int value){
        node * p;
           p = new node;
            p-> data = value;
        if(list== NULL){
            p-> next =NULL;
            list = p;
        }
        else {
            p->next=list;
            list = p;
        }
    }
    void insertion_at_end(int value){
         node * p;
           p = new node;
            p-> data = value;
        if(list== NULL){
            p-> next =NULL;
            list = p;
        }
        else{
            node * q = list;
            while (q-> next != NULL){
                q = q-> next;
            }
            p-> next =NULL;
            q->next = p;
        }
    }
    void insertion_at_specified(int position , int value){
         node * p = new node;;
        p-> data = value;
        if(list== NULL){
            p-> next =NULL;
            list = p;
        }
        else{
            node * q = list;
            while(q != NULL){
                if(q-> data == position){
                    q->next = p-> next;
                    q->next = p;
                }
                q = q->next;
                return;
            }
        }
    }
    void display(){
        node *q=list;
        while(q!= NULL){
            cout<<"  "<<q->data;
            q=q->next;
        }
    }
};
int main(){
    linked_list s;
    s.insertion_at_start(3);
    s.insertion_at_start(4);
    s.insertion_at_start(5);
    s.insertion_at_start(6);
    s.insertion_at_end(2);
    s.insertion_at_end(1);
    s.insertion_at_end(0);
    s.insertion_at_end(-1);
    s.insertion_at_specified(6,6);
    s.display();


}