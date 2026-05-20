#ifndef SCABS_BACKEND_H // Include guard — prevents this file from being included more than once
#define SCABS_BACKEND_H // Defines the guard macro so subsequent includes are skipped

// ============================================================
//  SCABS — Smart Classroom Availability & Booking System
//  Backend Header  |  DSA Project Semester 3
//
//  Data Structures used:
//   1. Linked List      — stores all Classroom objects
//   2. Hash Table       — O(1) lookup by Room ID
//   3. BST              — rooms sorted by ID
//   4. Queue            — FIFO booking requests
//   5. Stack            — undo last N actions
//   6. Priority Queue   — room recommendations
//   7. SlotNode List    — per-room timetable (replaces 2D array)
// ============================================================

#include <iostream>  // For cout/cin — standard input/output
#include <string>    // For std::string
using namespace std; // Avoids writing "std::" before cout, string, etc.

// ─── CONSTANTS ─────────────────────────────────────────────

const int DAYS = 7;        // 7 days in a week (Mon–Sun)
const int SLOTS = 8;       // 8 time slots per day (8 AM → 3 PM)
const int HASH_SIZE = 101; // Prime number for hash table size — primes reduce collision clustering
const int MAX_UNDO = 50;   // Maximum number of actions the undo stack can hold

// ─── FORWARD DECLARATIONS ──────────────────────────────────
// These tell the compiler these structs exist before their full definition below.
// Needed because Booking and Classroom may reference each other.

struct Classroom; // Forward-declare Classroom
struct Booking;   // Forward-declare Booking

// ============================================================
//  SlotNode — one booked (day, slot) pair
//  Instead of a 2D bool array timetable[DAYS][SLOTS],
//  each room holds a linked list of SlotNodes for only the
//  slots that are actually booked. Saves memory.
// ============================================================

struct SlotNode
{
    int day;        // Day index: 0=Mon, 1=Tue, ... 6=Sun
    int slot;       // Slot index: 0=8AM, 1=9AM, ... 7=3PM
    SlotNode *next; // Pointer to next SlotNode in the list (singly linked)
};

// ─── Timetable helper functions ────────────────────────────
// These are 'inline' so the compiler pastes the function body
// directly at each call site — avoids function-call overhead.

// isBooked: Walk the linked list to check if a (day, slot) is already booked.
// Time complexity: O(b) where b = number of booked slots in the room.

inline bool isBooked(
    SlotNode *head, // Head of this room's booked-slots list
    int day,        // Day to check
    int slot)       // Slot to check
{
    for (SlotNode *c = head; c != nullptr; c = c->next) // Traverse every node in the list
    {
        if (c->day == day && c->slot == slot) // If this node matches the target day AND slot
        {
            return true; // Slot is booked — return true immediately
        }
    }

    return false; // Reached end of list without finding it — slot is free
}

// bookSlot: Create a new SlotNode and insert it at the front of the list.
// Inserting at head is O(1) — no need to walk the whole list.

inline void bookSlot(
    SlotNode *&head, // Reference to head pointer (so we can update it)
    int day,         // Day being booked
    int slot)        // Slot being booked
{
    // Aggregate initializer: creates SlotNode with {day, slot, head}
    // The old head becomes the new node's next — classic prepend pattern
    SlotNode *n = new SlotNode{
        day,
        slot,
        head}; // new node's next points to old head

    head = n; // Head pointer now points to new node
}

// freeSlot: Find a SlotNode matching (day, slot) and remove it from the list.
// Time complexity: O(b) — may need to walk the whole list.

inline void freeSlot(
    SlotNode *&head, // Reference to head pointer (may change if head is deleted)
    int day,         // Day of slot to free
    int slot)        // Slot index to free
{
    if (!head) // If list is empty, nothing to remove
    {
        return;
    }

    // Special case: the target node is the head itself
    if (head->day == day && head->slot == slot)
    {
        SlotNode *d = head; // Save pointer to current head (to delete it)
        head = head->next;  // Move head forward to the next node
        delete d;           // Free memory of the removed node

        return;
    }

    // General case: walk the list looking for the node BEFORE the target
    for (SlotNode *p = head; p->next; p = p->next) // p is the "previous" node
    {
        if (p->next->day == day && p->next->slot == slot) // Found the node to delete
        {
            SlotNode *d = p->next; // Save pointer to target node
            p->next = d->next;     // Bridge around the target node
            delete d;              // Free target node's memory

            return;
        }
    }
    // If we reach here, the slot wasn't in the list — nothing happens (silent fail)
}

// ============================================================
//  CLASSROOM  — The main data entity
//  Each Classroom serves multiple roles simultaneously:
//    • Node in the Linked List  (via 'next')
//    • Node in the BST          (via 'left' and 'right')
//    • Node in the Hash Table   (via 'hashNext')
//  This avoids allocating separate wrapper nodes for each DS.
// ============================================================

struct Classroom
{
    int roomId;            // Unique integer ID, auto-assigned starting at 101
    string name;           // Human-readable room name e.g. "CS Lab 1"
    int capacity;          // Max number of students the room can hold
    bool hasAC;            // true if room has air conditioning
    float rating;          // Quality rating from 0.0 to 5.0
    string category;       // Computed label: "Good" (AC + rating>=3.5) or "Bad"
    SlotNode *bookedSlots; // Head of this room's linked list of booked time slots
    Classroom *next;       // Linked List: pointer to next classroom in the list
    Classroom *left;       // BST: pointer to left child (smaller roomId)
    Classroom *right;      // BST: pointer to right child (larger roomId)
    Classroom *hashNext;   // Hash Table: pointer to next item in the same bucket (chaining)
};

// ============================================================
//  BOOKING  — A single booking request node used in the Queue
// ============================================================

struct Booking
{
    int bookingId; // Unique booking ID, auto-assigned starting at 1001
    int roomId;    // Which room was booked
    int studentId; // ID of the student who made the booking
    int day;       // Day index (0–6)
    int slot;      // Slot index (0–7)
    Booking *next; // Pointer to next booking in the queue (singly linked)
};

// ============================================================
//  ACTION  — Stored on the Undo Stack to reverse operations
// ============================================================

struct Action
{
    string type;    // What action was performed: "BOOK" or "MARK_EMPTY"
    int roomId;     // Which room was affected
    int day;        // Which day
    int slot;       // Which slot
    int bookingId;  // Booking ID (relevant for BOOK actions; -1 otherwise)
    bool prevState; // The slot's state BEFORE the action (true=was booked, false=was free)
                    // Used to restore the previous state during undo
};

// ============================================================
//  DATA STRUCTURE 1 — LINKED LIST
//  Stores all Classroom objects in insertion order.
//  Provides O(n) search and O(1) head insertion.
// ============================================================

struct LinkedList
{
    Classroom *head; // Pointer to the first classroom in the list
    int count;       // Total number of classrooms in the list

    LinkedList() : head(nullptr), count(0) // Constructor: empty list, count = 0
    {
    }

    // insertAtHead: Add a new classroom at the front — O(1)
    void insertAtHead(Classroom *room)
    {
        room->next = head; // New room's next points to current head
        head = room;       // Update head to the new room
        count++;           // Increment count
    }

    // search: Walk the list to find a room by ID — O(n)
    Classroom *search(int roomId)
    {
        for (Classroom *c = head; c; c = c->next) // Traverse from head to tail
        {
            if (c->roomId == roomId) // Found a match
            {
                return c; // Return pointer to the matched room
            }
        }

        return nullptr; // Not found — return null
    }

    // remove: Find and remove a room by ID, then free its memory — O(n)
    bool remove(int roomId)
    {
        if (!head) // If the list is empty, nothing to remove
        {
            return false;
        }

        // Special case: the target is the head node
        if (head->roomId == roomId)
        {
            Classroom *d = head; // Save head pointer for deletion
            head = head->next;   // Move head forward
            delete d;            // Free the removed node
            count--;             // Decrement count

            return true;
        }

        // General case: search for the node just BEFORE the target
        for (Classroom *p = head; p->next; p = p->next)
        {
            if (p->next->roomId == roomId) // Found the node to delete
            {
                Classroom *d = p->next; // Save pointer to target
                p->next = d->next;      // Bridge around target
                delete d;               // Free target memory
                count--;

                return true;
            }
        }

        return false; // Room ID not found in list
    }

    // display: Print all rooms to console — O(n)
    void display()
    {
        if (!head) // If list is empty
        {
            cout << "  (no rooms)" << endl;
            return;
        }

        for (Classroom *c = head; c; c = c->next) // Walk the entire list
        {
            // Print each room's details on one line
            cout << "  [" << c->roomId << "] " << c->name
                 << " | Cap: " << c->capacity
                 << " | AC: " << (c->hasAC ? "Yes" : "No")
                 << " | Rating: " << c->rating
                 << " | " << c->category << endl;
        }
    }
};

// ============================================================
//  DATA STRUCTURE 2 — HASH TABLE  (separate chaining)
//  Maps roomId → Classroom* for O(1) average-case lookup.
//  Collision resolution: each bucket holds a linked list
//  using the Classroom's own 'hashNext' pointer.
// ============================================================

struct HashTable
{
    Classroom *buckets[HASH_SIZE]; // Array of bucket heads; each bucket is a chain of classrooms

    HashTable()
    {
        for (int i = 0; i < HASH_SIZE; i++) // Initialize all buckets to nullptr
        {
            buckets[i] = nullptr;
        }
    }

    // hash: Map a roomId to a bucket index — O(1)
    // The double-mod trick handles negative keys safely
    int hash(int key)
    {
        return (key % HASH_SIZE + HASH_SIZE) % HASH_SIZE; // Ensures result is always 0..HASH_SIZE-1
    }

    // insert: Place a room into its bucket at the front — O(1)
    void insert(Classroom *room)
    {
        int i = hash(room->roomId);  // Compute bucket index
        room->hashNext = buckets[i]; // New room's hashNext points to current bucket head
        buckets[i] = room;           // Bucket head is now this room
    }

    // search: Walk the chain in a bucket to find a room — O(1) average, O(n) worst
    Classroom *search(int roomId)
    {
        for (Classroom *c = buckets[hash(roomId)]; c; c = c->hashNext) // Walk bucket chain
        {
            if (c->roomId == roomId) // Match found
            {
                return c;
            }
        }

        return nullptr; // Not found
    }

    // remove: Unlink a room from its bucket chain — O(1) average
    void remove(int roomId)
    {
        int i = hash(roomId);      // Compute bucket index
        Classroom *c = buckets[i]; // Start at bucket head
        Classroom *p = nullptr;    // Previous pointer (for relinking)

        while (c)
        {
            if (c->roomId == roomId) // Found the room to remove
            {
                if (p)
                {
                    p->hashNext = c->hashNext; // Relink previous node past c
                }
                else
                {
                    buckets[i] = c->hashNext; // c was the head — update bucket head
                }

                return; // Note: does NOT delete c (Linked List owns the memory)
            }

            p = c;           // Advance previous
            c = c->hashNext; // Advance current
        }
    }
};

// ============================================================
//  DATA STRUCTURE 3 — BST  (Binary Search Tree)
//  Keeps classrooms sorted by roomId.
//  In-order traversal prints rooms in ascending ID order.
//  Uses the Classroom's own 'left'/'right' pointers.
// ============================================================

struct BST
{
    Classroom *root; // Root node of the BST

    BST() : root(nullptr) // Constructor: empty tree
    {
    }

    // insertNode: Recursively find the correct position and place the room — O(log n) avg
    Classroom *insertNode(
        Classroom *node, // Current node being examined
        Classroom *room) // Room to insert
    {
        if (!node) // Base case: found an empty spot — place the room here
        {
            return room;
        }

        if (room->roomId < node->roomId) // New room's ID is smaller — go left
        {
            node->left = insertNode(node->left, room);
        }
        else if (room->roomId > node->roomId) // New room's ID is larger — go right
        {
            node->right = insertNode(node->right, room);
        }
        // Equal IDs are ignored (no duplicates allowed)

        return node; // Return the unchanged current node (for recursive relinking)
    }

    // insert: Public wrapper — resets the room's BST pointers then calls insertNode
    void insert(Classroom *room)
    {
        room->left = room->right = nullptr; // Clear any stale left/right pointers
        root = insertNode(root, room);      // Insert and update root
    }

    // inorder: Recursive in-order traversal (Left → Node → Right) = sorted ascending
    void inorder(Classroom *node)
    {
        if (!node) // Base case: null node — stop recursion
        {
            return;
        }

        inorder(node->left); // Visit all smaller-ID nodes first

        // Print this node
        cout << "  [" << node->roomId << "] " << node->name
             << "  Rating: " << node->rating << endl;

        inorder(node->right); // Then visit all larger-ID nodes
    }

    // displaySorted: Print all rooms in ascending roomId order
    void displaySorted()
    {
        inorder(root); // Start in-order traversal from the root
    }

    // search: Recursively find a node by ID — O(log n) avg
    Classroom *search(
        Classroom *node, // Current node
        int id)          // Target room ID
    {
        if (!node || node->roomId == id) // Base cases: not found (null) or found (match)
        {
            return node;
        }

        // Decide which subtree to recurse into using ternary operator
        return id < node->roomId
                   ? search(node->left, id)   // Target is smaller — search left
                   : search(node->right, id); // Target is larger — search right
    }

    // find: Public wrapper that starts the search from the root
    Classroom *find(int id)
    {
        return search(root, id);
    }
};

// ============================================================
//  DATA STRUCTURE 4 — QUEUE  (FIFO booking requests)
//  Uses a singly linked list with both front and rear pointers
//  so enqueue (rear) and dequeue (front) are both O(1).
// ============================================================

struct Queue
{
    Booking *front; // Pointer to the first (oldest) booking — dequeue from here
    Booking *rear;  // Pointer to the last (newest) booking — enqueue here
    int size;       // Number of bookings currently in the queue

    Queue() : front(nullptr), rear(nullptr), size(0) // Constructor: empty queue
    {
    }

    // isEmpty: Returns true if queue has no elements
    bool isEmpty()
    {
        return !front; // If front is null, queue is empty
    }

    // enqueue: Add a new booking at the rear — O(1)
    void enqueue(Booking *b)
    {
        b->next = nullptr; // New node is always the last — it has no next

        if (!rear) // If queue is empty, both front and rear point to new node
        {
            front = rear = b;
        }
        else
        {
            rear->next = b; // Link current rear to new node
            rear = b;       // Update rear to new node
        }

        size++;
    }

    // dequeue: Remove and return the front booking — O(1)
    Booking *dequeue()
    {
        if (isEmpty()) // Can't dequeue from empty queue
        {
            return nullptr;
        }

        Booking *t = front;  // Save front pointer to return it
        front = front->next; // Move front to the next booking

        if (!front) // If queue is now empty
        {
            rear = nullptr; // Reset rear too (both must be null when empty)
        }

        size--;

        return t; // Return the removed booking
    }

    // peek: Return the front booking without removing it — O(1)
    Booking *peek()
    {
        return front;
    }

    // display: Print all bookings in queue order (front to rear)
    void display()
    {
        if (isEmpty())
        {
            cout << "  (empty queue)" << endl;

            return;
        }

        int pos = 1; // Position counter for display (1-indexed)

        for (Booking *c = front; c; c = c->next) // Walk from front to rear
        {
            cout << "  " << pos++ << ". BookingID=" << c->bookingId
                 << "  Room=" << c->roomId
                 << "  Student=" << c->studentId
                 << "  Day=" << c->day + 1 // +1 to convert 0-index to 1-index for display
                 << "  Slot=" << c->slot + 1 << endl;
        }
    }
};

// ============================================================
//  DATA STRUCTURE 5 — STACK  (undo history, array-based)
//  Stores the last MAX_UNDO actions for reversal.
//  Array-based (not linked list) for simple, cache-friendly access.
//  When full, oldest entry is dropped (sliding window behavior).
// ============================================================

struct Stack
{
    Action data[MAX_UNDO]; // Fixed-size array holding up to MAX_UNDO actions

    int top; // Index of the topmost (most recent) element; -1 means empty

    Stack() : top(-1) // Constructor: stack is empty
    {
    }

    bool isEmpty()
    {
        return top == -1; // Stack is empty when top hasn't moved from -1
    }

    bool isFull()
    {
        return top == MAX_UNDO - 1; // Stack is full when top reaches last valid index
    }

    // push: Add a new action to the top of the stack
    void push(Action a)
    {
        if (isFull())
        {
            // Shift all elements left by one to drop the oldest entry (index 0)
            for (int i = 0; i < MAX_UNDO - 1; i++)
            {
                data[i] = data[i + 1]; // Move each element one position back
            }

            data[top] = a; // Overwrite the last position with the new action
            // top stays the same — still MAX_UNDO-1
        }
        else
        {
            data[++top] = a; // Pre-increment top, then store the action
        }
    }

    // pop: Remove and return the top (most recent) action — O(1)
    Action pop()
    {
        Action e;
        e.type = "EMPTY";                   // Sentinel action returned when stack is empty
        return isEmpty() ? e : data[top--]; // Return top element and decrement top
    }

    // display: Print all actions, newest first (top → bottom)
    void display()
    {
        if (isEmpty())
        {
            cout << "  (stack empty)" << endl;

            return;
        }

        for (int i = top; i >= 0; i--) // Iterate from top down to 0
        {
            cout << "  [" << (top - i + 1) << "] " << data[i].type // Rank from top
                 << "  Room=" << data[i].roomId
                 << "  Day=" << data[i].day + 1
                 << "  Slot=" << data[i].slot + 1 << endl;
        }
    }
};

// ============================================================
//  DATA STRUCTURE 6 — PRIORITY QUEUE  (sorted linked list)
//  Recommends the best available room based on a computed score.
//  Score = rating + 2 (if AC) + 3 (if "Good" category).
//  Implemented as a sorted linked list: highest priority at head.
//  insert is O(n); extractMax is O(1).
// ============================================================

struct PQNode
{
    Classroom *room; // Pointer to the classroom this node represents
    float priority;  // Pre-computed score for this room
    PQNode *next;    // Next node in the sorted priority list
};

struct PriorityQueue
{
    PQNode *head; // Head of the list — always holds the highest-priority node
    int size;     // Number of nodes in the priority queue

    PriorityQueue() : head(nullptr), size(0) // Constructor: empty PQ
    {
    }

    bool isEmpty()
    {
        return !head;
    }

    // score: Compute a numeric priority for a room
    float score(Classroom *r)
    {
        float p = r->rating; // Base score = room's rating (0.0–5.0)

        if (r->hasAC)
        {
            p += 2.0f; // Bonus for air conditioning
        }

        if (r->category == "Good")
        {
            p += 3.0f; // Bonus for being a "Good" category room
        }

        return p; // Max possible score = 5.0 + 2.0 + 3.0 = 10.0
    }

    // insert: Add a room in sorted position (highest priority first) — O(n)
    void insert(Classroom *room)
    {
        PQNode *n = new PQNode();  // Allocate a new PQ node
        n->room = room;            // Link it to the room
        n->priority = score(room); // Compute and store its priority score
        n->next = nullptr;

        // Case 1: List is empty OR new node has higher priority than current head
        if (!head || n->priority > head->priority)
        {
            n->next = head; // New node points to old head
            head = n;       // New node becomes the new head
        }
        else
        {
            // Case 2: Walk the list to find the correct insertion point
            PQNode *c = head;

            // Move forward while the NEXT node has priority >= new node's priority
            while (c->next && c->next->priority >= n->priority)
            {
                c = c->next;
            }

            // Insert new node between c and c->next
            n->next = c->next;
            c->next = n;
        }

        size++;
    }

    // extractMax: Remove and return the highest-priority room — O(1)
    Classroom *extractMax()
    {
        if (isEmpty())
        {
            return nullptr;
        }

        PQNode *t = head;       // Save head (highest priority)
        Classroom *r = t->room; // Save the room pointer to return
        head = head->next;      // Advance head to second-highest
        delete t;               // Free the removed PQ node (NOT the classroom itself)
        size--;

        return r; // Return the best room
    }

    // clear: Remove all nodes from the PQ (cleanup)
    void clear()
    {
        while (!isEmpty())
        {
            extractMax(); // Keep extracting until empty — frees all PQNodes
        }
    }

    // display: Print all rooms in the PQ ranked by priority
    void display()
    {
        int rank = 1;

        for (PQNode *c = head; c; c = c->next) // Walk from highest to lowest priority
        {
            cout << "  #" << rank++ << " [" << c->room->roomId << "] "
                 << c->room->name << "  Score=" << c->priority
                 << "  Cat=" << c->room->category << endl;
        }
    }
};

// ============================================================
//  SCABS  — Main system class
//  Owns and coordinates all 6 data structures.
//  All public methods print results via cout.
//  The GUI captures cout to display output.
// ============================================================

class SCABS
{

private:
    LinkedList roomList; // DS1: Master list of all classrooms
    HashTable hashTable; // DS2: O(1) room lookup by ID
    BST bst;             // DS3: Rooms sorted by ID (for sorted display)
    Queue bookingQueue;  // DS4: FIFO queue of booking requests
    Stack undoStack;     // DS5: Stack of last MAX_UNDO actions for undo
    PriorityQueue pq;    // DS6: Re-used per recommend call; rooms scored by quality
    int nextRoomId;      // Auto-increment counter for room IDs (starts at 101)
    int nextBookingId;   // Auto-increment counter for booking IDs (starts at 1001)

    // Display-name arrays for converting 0-based indices to readable strings
    string dayNames[DAYS] = {"Mon", "Tue", "Wed", "Thu", "Fri", "Sat", "Sun"};
    string slotNames[SLOTS] = {"8AM", "9AM", "10AM", "11AM", "12PM", "1PM", "2PM", "3PM"};

    // categorize: Determine if a room is "Good" or "Bad" based on amenities
    string categorize(Classroom *r)
    {
        // "Good" only if it has AC AND a rating of at least 3.5
        return (r->hasAC && r->rating >= 3.5f) ? "Good" : "Bad";
    }

public:
    SCABS() : nextRoomId(101), nextBookingId(1001) // Constructor: init ID counters
    {
    }

    // ── addClassroom ─────────────────────────────────────────
    // Creates a new Classroom, registers it in all three structures
    // (Linked List, Hash Table, BST), and prints confirmation.

    void addClassroom(
        string name,  // Room's display name
        int capacity, // Max student capacity
        bool hasAC,   // Whether room has AC
        float rating) // Quality rating (0.0–5.0)
    {
        Classroom *room = new Classroom(); // Allocate a new Classroom on the heap

        room->roomId = nextRoomId++; // Assign and increment the ID counter
        room->name = name;
        room->capacity = capacity;
        room->hasAC = hasAC;
        room->rating = rating;
        room->category = categorize(room);                                // Compute "Good" or "Bad" label
        room->bookedSlots = nullptr;                                      // No slots booked yet
        room->next = room->left = room->right = room->hashNext = nullptr; // Clear all pointers

        roomList.insertAtHead(room); // Register in Linked List
        hashTable.insert(room);      // Register in Hash Table
        bst.insert(room);            // Register in BST

        cout << "  Room added: [" << room->roomId << "] " << name
             << "  Category: " << room->category << endl;
    }

    // ── bookRoom ─────────────────────────────────────────────
    // Books a specific (day, slot) in a room for a student.
    // Validates room existence, index bounds, and slot availability.
    // Records the booking in the Queue and the action on the Undo Stack.

    bool bookRoom(
        int roomId,    // Which room to book
        int studentId, // Who is booking it
        int day,       // Day index (0–6)
        int slot)      // Slot index (0–7)
    {
        Classroom *room = hashTable.search(roomId); // O(1) lookup via Hash Table

        if (!room)
        {
            cout << "  Room not found.\n";
            return false;
        }

        if (day < 0 || day >= DAYS) // Validate day index
        {
            cout << "  Invalid day.\n";
            return false;
        }

        if (slot < 0 || slot >= SLOTS) // Validate slot index
        {
            cout << "  Invalid slot.\n";
            return false;
        }

        if (isBooked(room->bookedSlots, day, slot)) // Check if slot is already taken
        {
            cout << "  Slot already booked!\n";
            return false;
        }

        bookSlot(room->bookedSlots, day, slot); // Mark the slot as booked in the room's list

        // Create a Booking record and add it to the Queue
        Booking *b = new Booking();
        b->bookingId = nextBookingId++; // Assign and increment booking ID
        b->roomId = roomId;
        b->studentId = studentId;
        b->day = day;
        b->slot = slot;
        b->next = nullptr;

        bookingQueue.enqueue(b); // Add booking to the FIFO queue (DS4)

        // Record this action on the Undo Stack (DS5)
        Action a;
        a.type = "BOOK";
        a.roomId = roomId;
        a.day = day;
        a.slot = slot;
        a.bookingId = b->bookingId;
        a.prevState = false; // Slot was FREE before this action — undo should free it again

        undoStack.push(a);

        cout << "  Booking confirmed! ID=" << b->bookingId << "\n";

        return true;
    }

    // ── markClassEmpty ───────────────────────────────────────
    // Manually frees a booked slot (e.g. class was cancelled).
    // Records the action on the Undo Stack so it can be reversed.

    void markClassEmpty(
        int roomId, // Room whose slot should be freed
        int day,    // Day index
        int slot)   // Slot index
    {
        Classroom *room = hashTable.search(roomId);

        if (!room)
        {
            cout << "  Room not found.\n";
            return;
        }

        if (!isBooked(room->bookedSlots, day, slot)) // Can't free a slot that isn't booked
        {
            cout << "  Slot is already free.\n";
            return;
        }

        freeSlot(room->bookedSlots, day, slot); // Remove the SlotNode from this room's list

        // Record this action for undo
        Action a;
        a.type = "MARK_EMPTY";
        a.roomId = roomId;
        a.day = day;
        a.slot = slot;
        a.bookingId = -1;   // No booking ID — this was a manual free, not a queue booking
        a.prevState = true; // Slot was BOOKED before this action — undo should re-book it

        undoStack.push(a);

        cout << "  Slot marked as empty. Room now available.\n";
    }

    // ── undoLastAction ───────────────────────────────────────
    // Pops the most recent Action from the Undo Stack and reverses it.
    // "BOOK" → free the slot. "MARK_EMPTY" → re-book the slot.

    void undoLastAction()
    {
        if (undoStack.isEmpty())
        {
            cout << "  Nothing to undo.\n";
            return;
        }

        Action a = undoStack.pop(); // Retrieve the most recent action

        Classroom *room = hashTable.search(a.roomId); // Look up the affected room

        if (!room)
        {
            cout << "  Cannot undo — room no longer exists.\n";
            return;
        }

        if (a.prevState) // If the slot was booked BEFORE the action, re-book it
        {
            bookSlot(room->bookedSlots, a.day, a.slot);
        }
        else // If the slot was free BEFORE the action, free it again
        {
            freeSlot(room->bookedSlots, a.day, a.slot);
        }

        cout << "  Undid: " << a.type << "  Room " << a.roomId
             << "  Day " << a.day + 1 << "  Slot " << a.slot + 1 << "\n";
    }

    // ── recommendRooms ───────────────────────────────────────
    // Finds all rooms free at a given (day, slot) and ranks them
    // using the Priority Queue by score (rating + AC + category).

    void recommendRooms(
        int day,  // Day to check availability for
        int slot) // Slot to check availability for
    {
        pq.clear(); // Reset the PQ from any previous recommendation call

        // Walk the Linked List and insert every free room into the PQ
        for (Classroom *c = roomList.head; c; c = c->next)
        {
            if (!isBooked(c->bookedSlots, day, slot)) // Only free rooms qualify
            {
                pq.insert(c); // Insert into PQ — auto-sorted by score
            }
        }

        if (pq.isEmpty())
        {
            cout << "  No rooms free at that time.\n";
            return;
        }

        cout << "\n  ═══ Recommended Rooms (Best First) ═══\n";

        int rank = 1;

        // Extract rooms one by one — PQ gives them in descending score order
        while (!pq.isEmpty())
        {
            Classroom *r = pq.extractMax(); // Get highest-scored room

            cout << "  #" << rank++ << " [" << r->roomId << "] " << r->name
                 << "  Cap=" << r->capacity
                 << "  AC=" << (r->hasAC ? "Yes" : "No")
                 << "  Rating=" << r->rating
                 << "  " << r->category << "\n";
        }
    }

    // ── displayRoomInfo ──────────────────────────────────────
    // Shows detailed info and the full weekly timetable for a room.
    // Timetable is rendered as a grid: rows=days, cols=slots.

    void displayRoomInfo(int roomId)
    {
        Classroom *room = hashTable.search(roomId);

        if (!room)
        {
            cout << "  Room not found.\n";
            return;
        }

        cout << "\n  ═══ Room Info ═══\n";
        cout << "  ID: " << room->roomId << "  Name: " << room->name << "\n";
        cout << "  Capacity: " << room->capacity
             << "  AC: " << (room->hasAC ? "Yes" : "No") << "\n";
        cout << "  Rating: " << room->rating
             << "  Category: " << room->category << "\n";

        cout << "\n  Timetable  ([ ] free  [X] booked)\n";

        // Print column headers (slot names)
        cout << "       ";
        for (int s = 0; s < SLOTS; s++)
        {
            cout << slotNames[s] << "\t";
        }
        cout << "\n";

        // Print each row (one per day), filling cells by checking the SlotNode list
        for (int d = 0; d < DAYS; d++)
        {
            cout << "  " << dayNames[d] << "  ";

            for (int s = 0; s < SLOTS; s++)
            {
                // isBooked scans the SlotNode list — prints [X] if found, [ ] if not
                cout << (isBooked(room->bookedSlots, d, s) ? "[X]\t" : "[ ]\t");
            }

            cout << "\n";
        }
    }

    // ── rescheduleClass ──────────────────────────────────────
    // Moves a booking from one (day, slot) to another within the same room.
    // Validates old slot is booked and new slot is free.

    void rescheduleClass(
        int roomId, // Room to reschedule within
        int od,     // Old day index
        int os,     // Old slot index
        int nd,     // New day index
        int ns)     // New slot index
    {
        Classroom *room = hashTable.search(roomId);

        if (!room)
        {
            cout << "  Room not found.\n";
            return;
        }

        if (!isBooked(room->bookedSlots, od, os)) // Old slot must be currently booked
        {
            cout << "  Old slot is not booked.\n";
            return;
        }

        if (isBooked(room->bookedSlots, nd, ns)) // New slot must be currently free
        {
            cout << "  New slot is already occupied.\n";
            return;
        }

        freeSlot(room->bookedSlots, od, os); // Remove old SlotNode
        bookSlot(room->bookedSlots, nd, ns); // Add new SlotNode

        cout << "  Class rescheduled successfully.\n";
    }

    // ── findFreeRooms ────────────────────────────────────────
    // Lists all rooms that are free at a given (day, slot).

    void findFreeRooms(
        int day,  // Day index to search
        int slot) // Slot index to search
    {
        cout << "\n  Free rooms on " << dayNames[day]
             << " at " << slotNames[slot] << ":\n";

        bool found = false;

        for (Classroom *c = roomList.head; c; c = c->next) // Walk all rooms
        {
            if (!isBooked(c->bookedSlots, day, slot)) // Room is free at this slot
            {
                cout << "  [" << c->roomId << "] " << c->name
                     << "  " << c->category << "\n";

                found = true;
            }
        }

        if (!found)
        {
            cout << "  No free rooms.\n";
        }
    }

    // ── checkAvailability ────────────────────────────────────
    // Reports whether a specific room is FREE or BOOKED at a given time.

    void checkAvailability(
        int roomId, // Room to check
        int day,    // Day index
        int slot)   // Slot index
    {
        Classroom *room = hashTable.search(roomId);

        if (!room)
        {
            cout << "  Room not found.\n";
            return;
        }

        bool free = !isBooked(room->bookedSlots, day, slot); // true if NOT in booked list

        cout << "  Room " << roomId << " on "
             << dayNames[day] << " " << slotNames[slot]
             << " is " << (free ? "FREE" : "BOOKED") << "\n";
    }

    // ── Display helpers ──────────────────────────────────────

    // displayAllRooms: Print every room via the Linked List
    void displayAllRooms()
    {
        cout << "\n  All Rooms:\n";
        roomList.display();
    }

    // displaySortedByID: Print all rooms in ascending ID order via BST in-order traversal
    void displaySortedByID()
    {
        cout << "\n  Rooms Sorted by ID:\n";
        bst.displaySorted();
    }

    // displayBookingQueue: Print all pending bookings front-to-rear
    void displayBookingQueue()
    {
        cout << "\n  Booking Queue (FIFO):\n";
        bookingQueue.display();
    }

    // displayUndoStack: Print all stored undo actions newest-first
    void displayUndoStack()
    {
        cout << "\n  Undo Stack:\n";
        undoStack.display();
    }

    // displayPriorityQueue: Print current PQ state (only populated after recommendRooms)
    void displayPriorityQueue()
    {
        cout << "\n  Priority Queue:\n";
        pq.display();
    }
};

#endif // SCABS_BACKEND_H  — End of include guard