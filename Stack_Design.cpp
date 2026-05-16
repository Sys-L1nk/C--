#include <iostream>
using namespace std;
#define size 4

 class stack{
    

    int arr[size];
    int top = -1;
    public:

    void push(int value){
        if (top == size -1)
        {
            cout<<"Over Flow !"<<endl;
        }
        else
        {
            top++;
            arr[top] = value;
        }
    }
    void pop(){
        if (top == -1)
        {
            cout<<" Under Flow !";
        }
        else{
            cout<<"The Vlaue poped is: "<< arr[top]<<endl;
            top--;
        }
        
    }
    void display(){
        if (top == -1)
        {
            cout<<" The Stack is empty !"<<endl;
        }
        else
        {
          cout<< " The stack contains: "<<endl;
          for (int i = top ; i >= 0 ; i--)
          {
            cout<<arr[i]<<" ";
            cout<<endl;
          }
          
            
        }
        
        
    }

};

int main (){

    stack s;
    s.push(23);
    s.push(4);
    s.push(66);
    s.push(87);
    s.push(43);
    s.push(60);
    s.pop();
    s.display();
    

}
