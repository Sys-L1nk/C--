#include <iostream>
using namespace std;

// Maximum plates needed to win
#define WIN_TARGET 5 
#define MAX_ATTEMPTS 10

class PlateStackGame {
private:
    int stack[WIN_TARGET];
    int topIndex = -1;
    
    int score = 0;
    int attempts = 0;
    bool collapsed = false;

public:

    void push(int weight) {
        attempts++;
        if (topIndex != -1) {
            if (weight > stack[topIndex]) {
                cout << "\n CRASH! You placed a weight of " << weight 
                     << " on top of " << stack[topIndex] << "." << endl;
                collapsed = true;
                return;
            }
        }
        
        stack[++topIndex] = weight;
        score += 2;
        cout << "\n Successfully added plate with weight " << weight << "." << endl;
    }

    void pop() {
        attempts++;
        
        if (topIndex == -1) {
            cout << "\n The stack is already empty! Nothing to pop." << endl;
            return;
        }
        
        cout << "\n Removed top plate with weight " << stack[topIndex] << "." << endl;
        topIndex--;
        score -= 1;
    }

    void peek() {
        if (topIndex == -1) {
            cout << "\n The stack is empty. No top plate to view." << endl;
        } else {
            cout << "\n The top plate currently has a weight of " << stack[topIndex] << "." << endl;
        }
    }


    void displayStack() {
        cout << "\n--- CURRENT STACK (" << getPlateCount() << " plates) ---" << endl;
        if (topIndex == -1) {
            cout << "      (Empty)" << endl;
        } else {
            for (int i = topIndex; i >= 0; i--) {
                cout << "     |  " << stack[i] << "  |";
                if (i == topIndex) cout << " <- TOP";
                cout << "\n";
            }
        }
        cout << "     -------" << endl;
    }

    bool isGameOver() {
        return collapsed || (topIndex + 1 == WIN_TARGET) || (attempts >= MAX_ATTEMPTS);
    }

    bool isWinner() {
        return (topIndex + 1 == WIN_TARGET) && !collapsed && (attempts <= MAX_ATTEMPTS);
    }

    int getPlateCount() {
        return topIndex + 1;
    }

    int getAttempts() {
        return attempts;
    }

    int getScore() {
        return score;
    }
};

int main() {
    PlateStackGame game;
    int choice;
    int weight;

    cout << "========================================" << endl;
    cout << "    PLATE STACK BALANCING GAME " << endl;
    cout << "========================================" << endl;
    cout << "Rules:" << endl;
    cout << "- Stack 5 plates to WIN." << endl;
    cout << "- Max 10 attempts allowed (Push/Pop)." << endl;
    cout << "- You CANNOT place a heavier plate on a lighter one." << endl;
    cout << "========================================\n" << endl;

    while (!game.isGameOver()) {
        cout << "\nAttempt: " << game.getAttempts() << "/" << MAX_ATTEMPTS 
             << " | Plates: " << game.getPlateCount() << "/5 | Score: " << game.getScore() << endl;
        cout << "1. Push (Add Plate)" << endl;
        cout << "2. Pop (Remove Plate)" << endl;
        cout << "3. Peek (View Top Plate)" << endl;
        cout << "Choose an action: ";
        cin >> choice;

        switch (choice) {
            case 1:
                cout << "Enter the weight of the plate: ";
                cin >> weight;
                game.push(weight);
                break;
            case 2:
                game.pop();
                break;
            case 3:
                game.peek();
                break;
            default:
                cout << "Invalid choice. Please enter 1, 2, or 3." << endl;
        }
    }

    cout << "\n========================================" << endl;
    cout << "               GAME OVER" << endl;
    cout << "========================================" << endl;

    if (game.isWinner()) {
        cout << " YOU WIN! You successfully stacked 5 plates!" << endl;
    } else {
        cout << " YOU LOSE! ";
        if (game.getPlateCount() < WIN_TARGET && game.getAttempts() >= MAX_ATTEMPTS) {
            cout << "You ran out of attempts." << endl;
        } else {
            cout << "Your stack collapsed." << endl;
        }
    }

    cout << "\n FINAL STATS " << endl;
    cout << "Final Score: " << game.getScore() << " points" << endl;
    cout << "Total Attempts: " << game.getAttempts() << endl;
    cout << "Plates Remaining: " << game.getPlateCount() << endl;
    game.displayStack();

    return 0;
}