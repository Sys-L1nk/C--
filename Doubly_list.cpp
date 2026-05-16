#include <iostream>
using namespace std;
struct Doubly_list
{
    int data;
    Doubly_list* next;
    Doubly_list* prev;

};

class Doubly_list
{
    
public:
void insert_at_specified_position(, int x)
{
   Doubly_list* list = NULL;
   Doubly_list* T = NULL;
   Doubly_list* P = NULL;
  
if (list == NULL)
{
    P = new Doubly_list;
    P->data = x;
    P->next = NULL;
    P->prev = NULL;
    list = P;
}
else
{
    P = new Doubly_list;
    P->data = x;
    P->next = NULL;
    P->prev = NULL;
    T->next = P;
    P->prev = T;
    T = P;


};
}

};

int main()
{
    cout << "Hello World!" << endl;
    return 0;
}