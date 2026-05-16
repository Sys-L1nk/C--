#include <iostream>
using namespace std;

#define MAX_SIZE 100

class MaxHeap
{
private:
    int heap[MAX_SIZE];
    int size;
    int parent(int i){ 
        return (i - 1) / 2;
    }
    int leftChild(int i) {
        return (2 * i) + 1; 
    }
    int rightChild(int i) { 
        return (2 * i) + 2; 
    }

public:
    MaxHeap()
    {
        size = 0;
    }
    void insert(int value)
    {
        if (size == MAX_SIZE)
        {
            cout << "Heap Overflow! Cannot insert." << endl;
            return;
        }
        int i = size;
        heap[i] = value;
        size++;
        while (i != 0 && heap[parent(i)] < heap[i])
        {
            swap(heap[i], heap[parent(i)]);
            i = parent(i);
        }
        cout << " Inserted " << value << endl;
    }
    void deleteRoot()
    {
        if (size == 0)
        {
            cout << "Heap Underflow! Nothing to delete." << endl;
            return;
        }

        cout << " Deleting the root (Max Value): " << heap[0] << endl;
        heap[0] = heap[size - 1];
        size--;
        int i = 0;

        while (true)
        {
            int left = leftChild(i);
            int right = rightChild(i);
            int largest = i;
            if (left < size && heap[left] > heap[largest])
            {
                largest = left;
            }
            if (right < size && heap[right] > heap[largest])
            {
                largest = right;
            }
            if (largest != i)
            {
                swap(heap[i], heap[largest]);
                i = largest; 
            }
            else
            {
                break;
            }
        }
    }

    
    void display()
    {
        if (size == 0)
        {
            cout << "The heap is empty!" << endl;
            return;
        }
        cout << " Current Heap Array: ";
        for (int i = 0; i < size; i++)
        {
            cout << heap[i] << " ";
        }
        cout << "\n"
             << endl;
    }
};

int main()
{
    MaxHeap h;

    cout << "--- BUILDING THE MAX HEAP ---" << endl;
    h.insert(10);
    h.insert(20);
    h.insert(5);
    h.insert(30);
    h.insert(15);

    h.display(); 

    cout << "--- DELETING FROM THE HEAP ---" << endl;
    h.deleteRoot();
    h.display();   

    h.deleteRoot();
    h.display();    

    return 0;
}