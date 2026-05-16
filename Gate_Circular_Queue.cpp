#include <iostream>
#include <string>

using namespace std;

#define SIZE 4 

class GateQueue {
public:
    string arr[SIZE];
    int F = -1;
    int R = -1;

    void enqueue(string gate) {
        if ((F == 0 && R == SIZE - 1) || (R + 1 == F)) {
            // Silently handle overflow for the simulation
            return;
        }
        else if (F == -1 && R == -1) {
            F = 0;
            R = 0;
            arr[R] = gate;
        }
        else if (R == SIZE - 1 && F != 0) {
            R = 0;
            arr[R] = gate;
        }
        else {
            R++;
            arr[R] = gate;
        }
    }

    string dequeue() {
        if (F == -1) {
            return "Empty"; 
        }

        string activeGate = arr[F];

        if (F == R) {
            F = -1;
            R = -1; 
        }
        else if (F == SIZE - 1) {
            F = 0; 
        }
        else {
            F++;
        }

        return activeGate;
    }
};

int main() {
    GateQueue parkingLot;

    parkingLot.enqueue("North");
    parkingLot.enqueue("East");
    parkingLot.enqueue("South");
    parkingLot.enqueue("West");

    int totalVehicles;
    cout << "Welcome to the Parking Lot System!" << endl;
    cout << "How many vehicles are waiting to enter? ";
    cin >> totalVehicles;
    
    cin.ignore(); 

    cout << "\n--- Starting Vehicle Entry Simulation ---" << endl;

    for (int i = 1; i <= totalVehicles; i++) {
        string plateNumber;
        
        cout << "\nEnter License Plate for Vehicle #" << i << ": ";
        getline(cin, plateNumber); 
        
        string currentGate = parkingLot.dequeue();
        
        cout << " Vehicle [" << plateNumber << "] has entered through the " << currentGate << " Gate." << endl;
        
        parkingLot.enqueue(currentGate);
    }

    cout << "\n All waiting vehicles have successfully entered the parking lot." << endl;

    return 0;
}
