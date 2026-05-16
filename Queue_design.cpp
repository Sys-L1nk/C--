#include <iostream>
using namespace std;

#define size 5 // Keeping it small to easily trigger the wrap-around

class CircularQueue {
public:
    int Q[size];
    int F = -1;
    int R = -1;

    // 1. ENQUEUE
    void enqueue(int value) {
        if ((F == 0 && R == size - 1) || (R + 1 == F)) {
            cout << " Over Flow! Cannot insert " << value << endl;
        }
        else if (F == -1 && R == -1) {
            F = 0;
            R = 0;
            Q[R] = value;
            cout << "Inserted " << value << " (First item)" << endl;
        }
        else if (R == size - 1 && F != 0) {
            R = 0;
            Q[R] = value;
            cout << " Inserted " << value << " (Wrapped around to index 0!)" << endl;
        }
        else {
            R++;
            Q[R] = value;
            cout << " Inserted " << value << endl;
        }
    }

    // 2. DEQUEUE
    int dequeue() {
        if (F == -1) {
            cout << " Under Flow! Nothing to dequeue." << endl;
            return -1; 
        }

        int dequeuedValue = Q[F];
        cout << " Dequeued: " << dequeuedValue << endl;

        if (F == R) {
            F = -1;
            R = -1; 
        }
        else if (F == size - 1) {
            F = 0; // Front wraps around
        }
        else {
            F++;
        }

        return dequeuedValue;
    }

    // 3. DISPLAY
    void display() {
        if (F == -1) {
            cout << "The Queue is empty!" << endl;
            return;
        }

        cout << " Queue elements: ";
        int i = F;
        while (i != R) {
            cout << Q[i] << " ";
            i = (i + 1) % size; // The modulo trick for printing
        }
        cout << Q[R] << endl; 
        cout << "   (Front is at index " << F << ", Rear is at index " << R << ")\n" << endl;
    }
};

// --- THE TEST RUN ---
int main() {
    CircularQueue cq;

    cout << "--- FILLING THE QUEUE ---" << endl;
    cq.enqueue(10);
    cq.enqueue(20);
    cq.enqueue(30);
    cq.enqueue(40);
    cq.enqueue(50);
    cq.display();

    cout << "--- FORCING OVERFLOW ---" << endl;
    cq.enqueue(60); // This should fail!
    cout << endl;

    cout << "--- REMOVING ITEMS (Opening space at the front) ---" << endl;
    cq.dequeue();
    cq.dequeue();
    cq.display();

    cout << "--- THE WRAP AROUND ---" << endl;
    cq.enqueue(60); 
    cq.enqueue(70); 
    cq.display();

    return 0;
}