#include <iostream>
using namespace std;

struct node {
    int data;
    node * next;
};

class Circular_Linked_List {
    node * list = NULL; // The head of the list

public:
    // 1. INSERT AT START
    void insertion_at_Start(int value) {
        node * newNode = new node;
        newNode->data = value;

        // If list is empty, it points to itself
        if (list == NULL) {
            list = newNode;
            newNode->next = list; 
        } 
        else {
            node * temp = list;
            
            // Travel to the very last node
            while (temp->next != list) {
                temp = temp->next;
            }
            
            // Wire the new node
            newNode->next = list;  // New node points to the current head
            temp->next = newNode;  // Last node points to the new node
            list = newNode;        // Finally, officially shift the head!
        }
    }

    // 2. INSERT AT END
    void insertion_at_end(int value) {
        node * newNode = new node;
        newNode->data = value;

        if (list == NULL) {
            list = newNode;
            newNode->next = list;
        } 
        else {
            node * temp = list;
            
            // Travel to the very last node
            while (temp->next != list) {
                temp = temp->next;
            }

            // Wire it to the end
            temp->next = newNode;  // Old last node points to new node
            newNode->next = list;  // New node loops back to the head
            
            // Notice we DO NOT change "list = newNode" here! 
            // The head stays exactly where it was.
        }
    }

    // 3. DELETE AT START
    void deletion_at_start() {
        if (list == NULL) {
            cout << "No Node to delete!" << endl;
        } 
        // Fixed the == bug. If there is only ONE node:
        else if (list->next == list) { 
            delete list;
            list = NULL; // Single equals sign to assign it!
        } 
        else {
            node * oldHead = list;
            node * temp = list;
            
            while (temp->next != list) {
                temp = temp->next;
            }
            
            list = list->next; 
            temp->next = list; 
        
            delete oldHead;   
        }
    }

    // 4. DELETE AT END
    void deletion_at_end() {
        if (list == NULL) {
            cout << "No Node to delete!" << endl;
        } 
        else if (list->next == list) {
            delete list;
            list = NULL; 
        } 
        else {
            node * current = list;
            node * previous = NULL;
        
            while (current->next != list) {
                previous = current;
                current = current->next;
            }
            previous->next = list; 
            
            delete current; 
        }
    }

    void display() {
        if (list == NULL) {
            cout << "The list is empty!" << endl;
            return;
        }

        node* temp = list;
        cout << "Circular Linked List: ";
        
        do {
            cout << temp->data << "  ";
            temp = temp->next;
        } while (temp != list); 
        
        cout << "(back to start)" << endl;
    }
};

int main() {
    Circular_Linked_List c;
    
    cout << "--- Inserting at Start ---" << endl;
    c.insertion_at_Start(4);
    c.insertion_at_Start(3);
    c.insertion_at_Start(2);
    c.insertion_at_Start(1); 
    c.display();
    
    cout << "\n--- Inserting at End ---" << endl;
    c.insertion_at_end(8);
    c.insertion_at_end(9);
    c.insertion_at_end(10); 
    c.display();
    
    cout << "\n--- Deleting at Start ---" << endl;
    c.deletion_at_start(); 
    c.display();
    
    cout << "\n--- Deleting at End ---" << endl;
    c.deletion_at_end(); 
    c.deletion_at_end();   
    c.display();

    return 0;
}