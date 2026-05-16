#include <iostream>
using namespace std;

class MaxHeap
{
private:
    int arr[100];
    int size;

public:
    MaxHeap()
    {
        size = 0;
    }

    void insert(int value)
    {
        size++;
        int index = size;
        arr[index] = value;

        while (index > 1)
        {
            int parent = index / 2;

            if (arr[parent] < arr[index])
            {
                swap(arr[parent], arr[index]);
                index = parent;
            }
            else
                break;
        }
    }

    void deleteRoot()
    {
        if (size == 0)
        {
            cout << "Heap is empty\n";
            return;
        }

        arr[1] = arr[size];
        size--;
        int i = 1;

        while (i <= size)
        {
            int left = 2 * i;
            int right = 2 * i + 1;
            int largest = i;

            if (left <= size && arr[left] > arr[largest])
                largest = left;

            if (right <= size && arr[right] > arr[largest])
                largest = right;

            if (largest != i)
            {
                swap(arr[i], arr[largest]);
                i = largest;
            }
            else
                break;
        }
    }

    void display()
    {
        for (int i = 1; i <= size; i++)
            cout << arr[i] << " ";
        cout << endl;
    }
};

int main()
{
    MaxHeap h;
    h.insert(50);
    h.insert(30);
    h.insert(40);
    h.insert(10);
    h.insert(20);

    cout << "Heap after insertion:\n";
    h.display();
    h.deleteRoot();

    cout << "Heap after deletion:\n";
    h.display();
}