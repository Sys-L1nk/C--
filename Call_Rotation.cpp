#include <iostream>
using namespace std;

// Node structure for each agent
class Node
{
public:
    string agentName;
    Node *next;

    Node(string name)
    {
        agentName = name;
        next = NULL;
    }
};

// Circular Linked List Class
class CallCenter
{
private:
    Node *head;
    Node *current;

public:
    CallCenter()
    {
        head = NULL;
        current = NULL;
    }

    // Add new agent
    void addAgent(string name)
    {
        Node *newNode = new Node(name);

        // If list is empty
        if (head == NULL)
        {
            head = newNode;
            newNode->next = head;
            current = head;
        }
        else
        {
            Node *temp = head;

            // Move to last node
            while (temp->next != head)
            {
                temp = temp->next;
            }

            temp->next = newNode;
            newNode->next = head;
        }

        cout << "Agent Added: " << name << endl;
    }

    // Display all agents
    void displayAgents()
    {
        if (head == NULL)
        {
            cout << "No agents available.\n";
            return;
        }

        Node *temp = head;

        cout << "\nAgents in Rotation:\n";

        do
        {
            cout << temp->agentName << " -> ";
            temp = temp->next;
        } while (temp != head);

        cout << "(Back to Start)\n";
    }

    // Assign incoming call
    void assignCall()
    {
        if (current == NULL)
        {
            cout << "No agents available to take calls.\n";
            return;
        }

        cout << "Incoming call assigned to: "
             << current->agentName << endl;

        // Move to next agent
        current = current->next;
    }
};

// Main Function
int main()
{
    CallCenter center;

    // Adding agents
    center.addAgent("Ali");
    center.addAgent("Ahmed");
    center.addAgent("Sara");
    center.addAgent("Ayesha");

    // Display agents
    center.displayAgents();

    cout << "\n--- Incoming Calls ---\n";

    // Simulating incoming calls
    center.assignCall();
    center.assignCall();
    center.assignCall();
    center.assignCall();
    center.assignCall();
    center.assignCall();

    return 0;
}