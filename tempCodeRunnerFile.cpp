#include <iostream>
using namespace std;

class Queue
{
    int arr[5];
    int front, rear, size;

public:
    Queue()
    {
        front = 0;
        rear = 4;
        size = 0;
    }

    bool isFull()
    {
        return size == 5;
    }

    bool isEmpty()
    {
        return size == 0;
    }

    void enqueue(int x)
    {
        if (isFull())
        {
            cout << "ENQ FAIL\n";
        }
        else
        {
            rear = (rear + 1) % 5;
            arr[rear] = x;
            size++;
            cout << "ENQ OK\n";
        }
    }

    void dequeue()
    {
        if (isEmpty())
        {
            cout << "DEQ FAIL\n";
        }
        else
        {
            int val = arr[front];
            front = (front + 1) % 5;
            size--;
            cout << "DEQ " << val << endl;
        }
    }

    void peek()
    {
        if (isEmpty())
        {
            cout << "PEEK FAIL\n";
        }
        else
        {
            cout << "PEEK " << arr[front] << endl;
        }
    }

    void printState()
    {
        cout << "ARR: ";
        for (int i = 0; i < 5; i++)
        {
            cout << arr[i] << " ";
        }
        cout << "| front=" << front
             << " rear=" << rear
             << " size=" << size << endl;
    }
};

int main()
{
    Queue q;

    q.enqueue(10);
    q.enqueue(20);
    q.enqueue(30);

    q.dequeue();

    q.enqueue(40);
    q.enqueue(50);
    q.enqueue(60);

    q.dequeue();

    q.enqueue(70);

    q.printState();

    q.peek();

    return 0;
}