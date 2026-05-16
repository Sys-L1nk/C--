#include <iostream>
#include <string>

using namespace std;

//  Defining the Node (Participant)
struct Participant
{
    int id;
    string name;
    string status; //  "Confirmed", "Pending", "Canceled"
    Participant *next;
};

class EventRegistration
{
private:
    Participant *head;

public:
    EventRegistration()
    {
        head = NULL;
    }

    // ---  REGISTER PARTICIPANT (Insert at End) ---
    void registerParticipant(int id, string name, string status)
    {
        Participant *newParticipant = new Participant;
        newParticipant->id = id;
        newParticipant->name = name;
        newParticipant->status = status;
        newParticipant->next = NULL;

        // If list is empty, make this the head
        if (head == NULL)
        {
            head = newParticipant;
        }
        // Otherwise, travel to the end and attach it
        else
        {
            Participant *temp = head;
            while (temp->next != NULL)
            {
                temp = temp->next;
            }
            temp->next = newParticipant;
        }
        cout << " Registered: " << name << " (ID: " << id << ") - Status: " << status << endl;
    }

    // ---  UPDATE REGISTRATION STATUS (Search and Modify) ---
    void updateStatus(int id, string newStatus)
    {
        Participant *temp = head;

        while (temp != NULL)
        {
            if (temp->id == id)
            {
                temp->status = newStatus;
                cout << " Updated ID " << id << " to status: " << newStatus << endl;
                return; // Exit once we find and update the participant
            }
            temp = temp->next;
        }
        cout << " Participant with ID " << id << " not found." << endl;
    }

    // ---  REMOVE CANCELED REGISTRATIONS (Delete Specific Nodes) ---
    void removeCanceled()
    {
        // Case 1: The head itself (or multiple consecutive heads) are "Canceled"
        while (head != NULL && head->status == "Canceled")
        {
            Participant *toDelete = head;
            head = head->next; 
            cout << " Removed canceled registration: " << toDelete->name << endl;
            delete toDelete;
        }

        // If the list becomes empty after removing heads, stop here
        if (head == NULL)
            return;

        // Case 2: Canceled participants in the middle or end of the list
        Participant *current = head;
        while (current->next != NULL)
        {
            if (current->next->status == "Canceled")
            {
                Participant *toDelete = current->next;
                // Wire the current node to the node *after* the one we are deleting
                current->next = current->next->next;
                cout << " Removed canceled registration: " << toDelete->name << endl;
                delete toDelete;
            }
            else
            {
                // Only move forward if we didn't just delete a node
                current = current->next;
            }
        }
    }

    // ---  DISPLAY ALL CONFIRMED PARTICIPANTS  ---
    void displayConfirmed()
    {
        cout << "\n---  Confirmed Participants ---" << endl;
        Participant *temp = head;
        bool found = false;

        while (temp != NULL)
        {
            if (temp->status == "Confirmed")
            {
                cout << "ID: " << temp->id << " | Name: " << temp->name << endl;
                found = true;
            }
            temp = temp->next;
        }

        if (!found)
        {
            cout << "No confirmed participants found." << endl;
        }
        cout << "---------------------------------\n"
             << endl;
    }
};

// --- MAIN FUNCTION TO TEST THE SYSTEM ---
int main()
{
    EventRegistration event;

    cout << "---  REGISTRATION PHASE ---" << endl;
    event.registerParticipant(101, "Ahmad", "Confirmed");
    event.registerParticipant(102, "Papu", "Pending");
    event.registerParticipant(103, "Sheikh", "Canceled");
    event.registerParticipant(104, "Ladu", "Pending");
    event.registerParticipant(105, "Rana", "Canceled");

    // Display only those currently confirmed
    event.displayConfirmed();

    cout << "---  STATUS UPDATES ---" << endl;
    // Ahmad pays his fee, update to Confirmed
    event.updateStatus(102, "Confirmed");
    // Ladu decides not to go, update to Canceled
    event.updateStatus(104, "Canceled");

    // Display to verify Ahmad is now on the list
    event.displayConfirmed();

    cout << "---  CLEANUP PHASE ---" << endl;
    // Remove all participants marked as "Canceled" (Sheikh and Rana, and now Ladu)
    event.removeCanceled();

    return 0;
}