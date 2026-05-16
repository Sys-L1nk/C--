#include <iostream>
#include <string> // Added to handle full words/expressions

using namespace std;
#define size 15

class Stack {
public:
    char arr[size];
    int top = -1;

    // Changed to void, and we increment top BEFORE adding the value
    void push(char value) {
        if (top == size - 1) {
            cout << "Over Flow !" << endl;
        } else {
            top++;            // Increment first!
            arr[top] = value; // Then assign
        }
    }

    // Changed to return the popped character, and fixed the == sign
    char pop() {
        if (top == -1) {
            return '\0'; // Return a null character if empty
        } else {
            char poppedValue = arr[top];
            top--;
            return poppedValue; // We need this value to compare later!
        }
    }

    // A quick helper to check if the stack is empty
    bool isEmpty() {
        return top == -1;
    }
};

int main() {
    string input; // Using string to capture the whole expression
    Stack s;
    bool isBalanced = true;

    cout << "Enter the brackets expression you want to check: ";
    cin >> input;

    // Loop through every character in the string expression
    for (int i = 0; i < input.length(); i++) {
        char current = input[i];

        // 1. If we see an OPENING bracket, push it to the stack
        if (current == '(' || current == '[' || current == '{') {
            s.push(current);
        }
        // 2. If we see a CLOSING bracket, pop from stack and compare
        else if (current == ')' || current == ']' || current == '}') {
            
            // If we have a closing bracket but the stack is empty, it's unbalanced
            if (s.isEmpty()) {
                isBalanced = false;
                break;
            }

            // Pop the last opening bracket we saw
            char topChar = s.pop();

            // Check if they are a mismatched pair
            if ((current == ')' && topChar != '(') ||
                (current == ']' && topChar != '[') ||
                (current == '}' && topChar != '{')) {
                isBalanced = false;
                break; // Stop looking, we already know it's unbalanced
            }
        }
    }

    // 3. Final check: It is perfectly balanced if we found no mismatches 
    // AND there are no leftover opening brackets in the stack
    if (isBalanced && s.isEmpty()) {
        cout << "The expression is balanced !" << endl;
    } else {
        cout << "The expression is unbalanced !" << endl;
    }

    return 0;
}