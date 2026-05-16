#include <iostream>
#include <string>

using namespace std;
#define size 15

class Stack {
public:
    char arr[size];
    int top = -1;

    void push(char value) {
        if (top == size - 1) {
            cout << "Over Flow !" << endl;
        } else {
            top++;            
            arr[top] = value; 
        }
    }

    char pop() {
        if (top == -1) {
            return '\0'; 
        } else {
            char poppedValue = arr[top];
            top--;
            return poppedValue; 
        }
    }

    bool isEmpty() {
        return top == -1;
    }

    // Helper to look at the top value without popping it
    char peek() {
        if (top == -1) return '\0';
        return arr[top];
    }
};

// 1. MOVED OUTSIDE OF MAIN: The precedence scoring function
int precedence(char op) {
    if (op == '^') {
        return 3; 
    }
    else if (op == '*' || op == '/') {
        return 2; 
    }
    else if (op == '+' || op == '-') {
        return 1; 
    }
    else {
        return -1; 
    }
}

int main() {
    string input;
    string postfix = "";
    Stack s;

    // 2. ADDED BACK USER INPUT
    cout << "Enter the Infix Expression: ";
    cin >> input;

    for (int i = 0; i < input.length(); i++) {
        char current = input[i];

        // RULE A: If it's an alphanumeric operand (A-Z, a-z, 0-9), add to postfix
        if ((current >= 'a' && current <= 'z') || 
            (current >= 'A' && current <= 'Z') || 
            (current >= '0' && current <= '9')) {
            postfix += current;
        }
        // RULE B: If it's an opening parenthesis, push it
        else if (current == '(') {
            s.push(current);
        }
        // RULE C: If it's a closing parenthesis, pop until '('
        else if (current == ')') {
            while (!s.isEmpty() && s.peek() != '(') {
                postfix += s.pop();
            }
            s.pop(); // Throw away the '('
        }
        // RULE D: If it is an operator
        else if (current == '+' || current == '-' || current == '*' || current == '/' || current == '^') {
            
            // 3. THE PRECEDENCE BATTLE
            // While stack isn't empty AND top of stack is >= current operator
            while (!s.isEmpty() && precedence(s.peek()) >= precedence(current)) {
                postfix += s.pop();
            }
            s.push(current); // Finally, push the current operator
        }
    }

    // 4. FINAL CLEANUP: Pop any remaining operators left in the stack
    while (!s.isEmpty()) {
        postfix += s.pop();
    }

    // Print the final result!
    cout << "The Postfix Expression is: " << postfix << endl;

    return 0;
}