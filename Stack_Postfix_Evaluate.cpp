#include <iostream>
#include <string>
#include <cmath> 
#include <stdexcept> // 1. Added this library to use standard C++ exceptions!

using namespace std;
#define size 15

class Stack {
public:
    int arr[size]; 
    int top = -1;

    void push(int value) {
        if (top == size - 1) {
            // Throw an exception instead of just printing
            throw overflow_error("Stack Overflow! Expression is too long.");
        } else {
            top++;            
            arr[top] = value; 
        }
    }

    int pop() {
        if (top == -1) {
            // 2. ERROR 1: If we try to pop an empty stack, there are too many operators!
            throw runtime_error("Invalid Expression: Too many operators!"); 
        } else {
            int poppedValue = arr[top];
            top--;
            return poppedValue; 
        }
    }

    bool isEmpty() {
        return top == -1;
    }
};

int main(){
    Stack s;
    string input;

    cout << "Enter the Postfix Expression: ";
    cin >> input;

    // 3. Start the TRY block. We will "try" to run this math.
    try {
        for (int i = 0; i < input.length(); i++)
        {
            char current = input[i];
            
            if (current >= '0' && current <= '9') {
                int number = current - '0';
                s.push(number);
            }
            else if (current == '+' || current == '-' || current == '*' || current == '/' || current == '^' || current == '%') {
                
                int firstvalue = s.pop();
                int secondvalue = s.pop();
                int resultvalue = 0;

                switch (current) {
                case '+': resultvalue = secondvalue + firstvalue; break;
                case '-': resultvalue = secondvalue - firstvalue; break;
                case '*': resultvalue = secondvalue * firstvalue; break;
                case '^': resultvalue = pow(secondvalue, firstvalue); break;
                
                case '/':
                    // 4. ERROR 3: Division by zero check
                    if (firstvalue == 0) throw runtime_error("Math Error: Division by zero!");
                    resultvalue = secondvalue / firstvalue;
                    break;

                case '%':
                    if (firstvalue == 0) throw runtime_error("Math Error: Modulo by zero!");
                    resultvalue = secondvalue % firstvalue;
                    break;
                }
                s.push(resultvalue);
            }
            // Ignore empty spaces if the user types them
            else if (current != ' ') {
                // 5. ERROR 4: Invalid character check
                throw runtime_error("Invalid Character: Only numbers and basic math operators are allowed.");
            }
        }

        // 6. ERROR 2: Leftover check. 
        // If the math is done, the top index should be exactly 0 (one item left). 
        if (s.top != 0) {
            throw runtime_error("Invalid Expression: Too many numbers, not enough operators!");
        }

        cout << "The final evaluated answer is: " << s.pop() << endl;

    } 
    // 7. CATCH block. If any "throw" happens above, the code instantly jumps here.
    catch (const exception& e) {
        cout << "\n❌ ERROR: " << e.what() << endl;
    }

    return 0;
}