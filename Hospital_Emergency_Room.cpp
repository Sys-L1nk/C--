#include <iostream>
#include <string>

using namespace std;

// ==========================================
// 1. DATA STRUCTURES
// ==========================================

struct Patient
{
    int id;
    string name;
    int severityScore;

    // Default constructor for array initialization
    Patient() : id(0), name(""), severityScore(0) {}
    Patient(int i, string n, int s) : id(i), name(n), severityScore(s) {}
};

struct Ambulance
{
    string callSign;
    int eta; // Estimated Time of Arrival in minutes

    Ambulance() : callSign(""), eta(0) {}
    Ambulance(string c, int t) : callSign(c), eta(t) {}
};

// ==========================================
// 2. PATIENT MAX-HEAP (High Score = Priority)
// ==========================================

class EmergencyWard
{
private:
    Patient *heap;
    int capacity;
    int size;

    // Helper math functions to find nodes in an array
    int parent(int i) { return (i - 1) / 2; }
    int leftChild(int i) { return (2 * i) + 1; }
    int rightChild(int i) { return (2 * i) + 2; }

    // Heapify Up: Used during Insertion
    void bubbleUp(int i)
    {
        while (i != 0 && heap[parent(i)].severityScore < heap[i].severityScore)
        {
            swap(heap[i], heap[parent(i)]);
            i = parent(i);
        }
    }

    // Heapify Down: Used during Deletion
    void bubbleDown(int i)
    {
        int largest = i;
        int left = leftChild(i);
        int right = rightChild(i);

        if (left < size && heap[left].severityScore > heap[largest].severityScore)
            largest = left;

        if (right < size && heap[right].severityScore > heap[largest].severityScore)
            largest = right;

        if (largest != i)
        {
            swap(heap[i], heap[largest]);
            bubbleDown(largest); // Recursive call
        }
    }

public:
    // Constructor (Dynamically allocates memory)
    EmergencyWard(int maxCapacity)
    {
        capacity = maxCapacity;
        size = 0;
        heap = new Patient[capacity]; // Allocation
    }

    // Destructor (Prevents memory leaks)
    ~EmergencyWard()
    {
        delete[] heap;
    }

    // Task: Register a new patient
    void admitPatient(int id, string name, int severity)
    {
        // Error Checking: Prevent Overflow
        if (size == capacity)
        {
            cout << " Error: Emergency Ward is full! Cannot admit " << name << endl;
            return;
        }

        Patient newPatient(id, name, severity);
        heap[size] = newPatient; // Place at the end of the array
        bubbleUp(size);          // Restore Max-Heap property
        size++;

        cout << " Admitted Patient: " << name << " (Severity: " << severity << ")" << endl;
    }

    // Task: Treat the most critical patient
    void treatNextPatient()
    {
        // Error Checking: Prevent Underflow
        if (size == 0)
        {
            cout << " No patients waiting in the Emergency Ward." << endl;
            return;
        }

        Patient criticalPatient = heap[0];
        cout << "🩺 Treating Patient: " << criticalPatient.name
             << " (ID: " << criticalPatient.id << ", Severity: " << criticalPatient.severityScore << ")" << endl;

        // Replace root with the last element and shrink size
        heap[0] = heap[size - 1];
        size--;

        // Restore Max-Heap property
        bubbleDown(0);
    }
};

// ==========================================
// 3. AMBULANCE MIN-HEAP (Low ETA = Priority)
// ==========================================

class DispatchSystem
{
private:
    Ambulance *heap;
    int capacity;
    int size;

    int parent(int i) { return (i - 1) / 2; }
    int leftChild(int i) { return (2 * i) + 1; }
    int rightChild(int i) { return (2 * i) + 2; }

    // Heapify Up: Used during Insertion (Looks for SMALLER values)
    void bubbleUp(int i)
    {
        while (i != 0 && heap[parent(i)].eta > heap[i].eta)
        {
            swap(heap[i], heap[parent(i)]);
            i = parent(i);
        }
    }

    // Heapify Down: Used during Deletion
    void bubbleDown(int i)
    {
        int smallest = i;
        int left = leftChild(i);
        int right = rightChild(i);

        if (left < size && heap[left].eta < heap[smallest].eta)
            smallest = left;

        if (right < size && heap[right].eta < heap[smallest].eta)
            smallest = right;

        if (smallest != i)
        {
            swap(heap[i], heap[smallest]);
            bubbleDown(smallest); // Recursive call
        }
    }

public:
    // Constructor
    DispatchSystem(int maxCapacity)
    {
        capacity = maxCapacity;
        size = 0;
        heap = new Ambulance[capacity];
    }

    // Destructor (Prevents memory leaks)
    ~DispatchSystem()
    {
        delete[] heap;
    }

    // Task: Log a new available ambulance
    void logAmbulance(string callSign, int eta)
    {
        if (size == capacity)
        {
            cout << " Error: Dispatch tracking is full!" << endl;
            return;
        }

        Ambulance newAmbulance(callSign, eta);
        heap[size] = newAmbulance;
        bubbleUp(size);
        size++;

        cout << " Logged Ambulance: " << callSign << " (ETA: " << eta << " mins)" << endl;
    }

    // Task: Dispatch the closest ambulance
    void dispatchNext()
    {
        if (size == 0)
        {
            cout << " Critical: No ambulances available for dispatch!" << endl;
            return;
        }

        Ambulance closestAmbulance = heap[0];
        cout << " Dispatching Ambulance: " << closestAmbulance.callSign
             << " (ETA: " << closestAmbulance.eta << " mins)" << endl;

        heap[0] = heap[size - 1];
        size--;
        bubbleDown(0);
    }
};

// ==========================================
// 4. MAIN FUNCTION (System Testing)
// ==========================================

int main()
{
    cout << "=== CITYCARE HOSPITAL EMERGENCY SYSTEM ===\n"
         << endl;

    // Initialize systems with capacity for 100 entries each
    EmergencyWard ward(100);
    DispatchSystem dispatch(100);

    cout << "--- Admitting Patients ---" << endl;
    ward.admitPatient(101, "Alice Smith", 5);   // Minor injury
    ward.admitPatient(102, "Bob Johnson", 9);   // Critical heart attack
    ward.admitPatient(103, "Charlie Davis", 7); // Broken arm

    cout << "\n--- Treating Patients ---" << endl;
    // Should treat Bob (9), then Charlie (7), then Alice (5)
    ward.treatNextPatient();
    ward.treatNextPatient();

    cout << "\n--- Logging Ambulances ---" << endl;
    dispatch.logAmbulance("Rescue-1", 15);
    dispatch.logAmbulance("Rescue-2", 5);
    dispatch.logAmbulance("Rescue-3", 8);

    cout << "\n--- Dispatching Ambulances ---" << endl;
    // Should dispatch Rescue-2 (5 mins), then Rescue-3 (8 mins)
    dispatch.dispatchNext();
    dispatch.dispatchNext();

    cout << "\n==========================================" << endl;
    return 0; // Destructors are automatically called here, preventing memory leaks!
}