/*
 * ============================================================
 *   SCABS — Smart Classroom Availability & Booking System
 *   A fully DSA-driven C++ console application
 *
 *   Data Structures (all manual):
 *     - Linked List      : classroom storage
 *     - BST              : sorted room lookup
 *     - Hash Table       : O(1) room / booking access
 *     - Queue (FIFO)     : booking requests
 *     - Stack            : undo operations
 *     - Priority Queue   : room recommendations (Max-Heap)
 *     - 2-D Array        : weekly timetable per room
 *
 *   GUI: Windows Console API + box-drawing characters
 *   Compiler: g++ / MSVC (Windows only)
 *   No external libraries, no STL containers.
 * ============================================================
 */

#include <iostream>
#include <windows.h>
#include <conio.h>
#include <cstring>
#include <cstdlib>

using namespace std;

// ============================================================
//  GLOBAL CONSTANTS
// ============================================================
namespace Const
{
    const int DAYS = 5;        // Mon – Fri
    const int SLOTS = 8;       // slots per day
    const int HASH_SIZE = 211; // prime
    const int MAX_UNDO = 60;
    const int MAX_ROOMS = 120;
    const int MAX_BOOKINGS = 600;
    const int SCR_W = 120;
    const int SCR_H = 40;
}

// ============================================================
//  UTILITY  (no <string>)
// ============================================================
namespace Util
{
    int strLen(const char *s)
    {
        int i = 0;
        while (s[i])
            i++;
        return i;
    }
    void strCpy(char *d, const char *s)
    {
        int i = 0;
        while ((d[i] = s[i]))
            i++;
    }
    bool strEq(const char *a, const char *b)
    {
        while (*a && *b)
        {
            if (*a != *b)
                return false;
            a++;
            b++;
        }
        return *a == *b;
    }
    int strCmp(const char *a, const char *b)
    {
        while (*a && *b)
        {
            if (*a < *b)
                return -1;
            if (*a > *b)
                return 1;
            a++;
            b++;
        }
        if (*a)
            return 1;
        if (*b)
            return -1;
        return 0;
    }
    void strCat(char *d, const char *s)
    {
        int i = Util::strLen(d);
        int j = 0;
        while ((d[i++] = s[j++]))
            ;
    }
    void intStr(int n, char *buf)
    {
        if (n == 0)
        {
            buf[0] = '0';
            buf[1] = 0;
            return;
        }
        bool neg = (n < 0);
        if (neg)
            n = -n;
        char t[24];
        int i = 0;
        while (n > 0)
        {
            t[i++] = '0' + (n % 10);
            n /= 10;
        }
        if (neg)
            t[i++] = '-';
        int j = 0;
        while (i > 0)
            buf[j++] = t[--i];
        buf[j] = 0;
    }
    int strInt(const char *s)
    {
        int r = 0, sign = 1, i = 0;
        if (s[0] == '-')
        {
            sign = -1;
            i = 1;
        }
        while (s[i] && s[i] >= '0' && s[i] <= '9')
        {
            r = r * 10 + (s[i] - '0');
            i++;
        }
        return sign * r;
    }
    // Parse simple decimal string → float
    float strFloat(const char *s)
    {
        float r = 0;
        int dotPos = -1;
        for (int i = 0; s[i]; i++)
        {
            if (s[i] == '.')
            {
                dotPos = i;
                continue;
            }
            if (dotPos < 0)
                r = r * 10 + (s[i] - '0');
        }
        if (dotPos >= 0)
        {
            float dec = 0.1f;
            for (int i = dotPos + 1; s[i]; i++)
            {
                r += (s[i] - '0') * dec;
                dec *= 0.1f;
            }
        }
        return r;
    }
    void fillChars(char *buf, char c, int n)
    {
        for (int i = 0; i < n; i++)
            buf[i] = c;
        buf[n] = 0;
    }
}

// ============================================================
//  GUI  — Windows Console wrapper
// ============================================================
namespace GUI
{
    enum Color
    {
        BLACK = 0,
        BLUE = 1,
        GREEN = 2,
        CYAN = 3,
        RED = 4,
        MAGENTA = 5,
        YELLOW = 6,
        WHITE = 7,
        BBLACK = 8,
        BBLUE = 9,
        BGREEN = 10,
        BCYAN = 11,
        BRED = 12,
        BMAGENTA = 13,
        BYELLOW = 14,
        BWHITE = 15
    };

    HANDLE hCon;

    void init()
    {
        hCon = GetStdHandle(STD_OUTPUT_HANDLE);
        SMALL_RECT wr = {0, 0, (SHORT)(Const::SCR_W - 1), (SHORT)(Const::SCR_H - 1)};
        SetConsoleWindowInfo(hCon, TRUE, &wr);
        COORD bsz = {Const::SCR_W, Const::SCR_H};
        SetConsoleScreenBufferSize(hCon, bsz);
        // Set title
        SetConsoleTitleA("SCABS - Smart Classroom Availability & Booking System");
        // Hide cursor by default
        CONSOLE_CURSOR_INFO ci;
        ci.dwSize = 1;
        ci.bVisible = FALSE;
        SetConsoleCursorInfo(hCon, &ci);
    }

    void cursor(bool show)
    {
        CONSOLE_CURSOR_INFO ci;
        ci.dwSize = 1;
        ci.bVisible = show ? TRUE : FALSE;
        SetConsoleCursorInfo(hCon, &ci);
    }

    void color(Color fg, Color bg = BLACK)
    {
        SetConsoleTextAttribute(hCon, (WORD)((bg << 4) | fg));
    }

    void at(int x, int y)
    {
        COORD p = {(SHORT)x, (SHORT)y};
        SetConsoleCursorPosition(hCon, p);
    }

    void cls()
    {
        COORD tl = {0, 0};
        DWORD w;
        CONSOLE_SCREEN_BUFFER_INFO i;
        GetConsoleScreenBufferInfo(hCon, &i);
        DWORD cells = (DWORD)(i.dwSize.X * i.dwSize.Y);
        FillConsoleOutputCharacterA(hCon, ' ', cells, tl, &w);
        FillConsoleOutputAttribute(hCon, i.wAttributes, cells, tl, &w);
        SetConsoleCursorPosition(hCon, tl);
    }

    void put(int x, int y, const char *s, Color fg = WHITE, Color bg = BLACK)
    {
        at(x, y);
        color(fg, bg);
        cout << s;
    }

    void putc2(int x, int y, char c, Color fg, Color bg = BLACK)
    {
        at(x, y);
        color(fg, bg);
        cout << c;
    }

    // center text inside a span starting at sx, width w
    void putC(int sx, int y, int w, const char *s, Color fg, Color bg = BLACK)
    {
        int len = Util::strLen(s);
        int x = sx + (w - len) / 2;
        if (x < sx)
            x = sx;
        put(x, y, s, fg, bg);
    }

    void hline(int x, int y, int w, Color fg, unsigned char ch = 196)
    {
        at(x, y);
        color(fg);
        for (int i = 0; i < w; i++)
            cout << ch;
    }

    void fillRect(int x, int y, int w, int h, Color fg = WHITE, Color bg = BLACK)
    {
        color(fg, bg);
        for (int r = 0; r < h; r++)
        {
            at(x, y + r);
            for (int c = 0; c < w; c++)
                cout << ' ';
        }
    }

    // Single-line box
    void box(int x, int y, int w, int h, Color fg)
    {
        color(fg);
        at(x, y);
        cout << (char)218;
        for (int i = 1; i < w - 1; i++)
            cout << (char)196;
        cout << (char)191;
        for (int r = 1; r < h - 1; r++)
        {
            at(x, y + r);
            cout << (char)179;
            at(x + w - 1, y + r);
            cout << (char)179;
        }
        at(x, y + h - 1);
        cout << (char)192;
        for (int i = 1; i < w - 1; i++)
            cout << (char)196;
        cout << (char)217;
    }

    // Double-line box
    void dbox(int x, int y, int w, int h, Color fg)
    {
        color(fg);
        at(x, y);
        cout << (char)201;
        for (int i = 1; i < w - 1; i++)
            cout << (char)205;
        cout << (char)187;
        for (int r = 1; r < h - 1; r++)
        {
            at(x, y + r);
            cout << (char)186;
            at(x + w - 1, y + r);
            cout << (char)186;
        }
        at(x, y + h - 1);
        cout << (char)200;
        for (int i = 1; i < w - 1; i++)
            cout << (char)205;
        cout << (char)188;
    }

    void header()
    {
        fillRect(0, 0, Const::SCR_W, 3, BWHITE, BLUE);
        color(BYELLOW, BLUE);
        at(0, 0);
        for (int i = 0; i < Const::SCR_W; i++)
            cout << (char)220;
        at(0, 2);
        for (int i = 0; i < Const::SCR_W; i++)
            cout << (char)223;
        putC(0, 1, Const::SCR_W, " SMART CLASSROOM AVAILABILITY & BOOKING SYSTEM (SCABS) ", BYELLOW, BLUE);
    }

    void footer()
    {
        color(BCYAN);
        at(0, 38);
        for (int i = 0; i < Const::SCR_W; i++)
            cout << (char)205;
        putC(0, 39, Const::SCR_W,
             "Linked List | BST | Hash Table | Queue | Stack | Priority Queue | 2-D Timetable",
             CYAN);
    }

    void banner()
    {
        cls();
        header();
        footer();
    }

    // Status bar row 36–37
    void status(const char *msg, Color fg = BGREEN)
    {
        fillRect(0, 36, Const::SCR_W, 2);
        put(2, 36, msg, fg);
    }
    void errMsg(const char *m) { status(m, BRED); }
    void okMsg(const char *m) { status(m, BGREEN); }

    // Titled section line
    void section(int y, const char *title, Color fg = BCYAN)
    {
        hline(2, y, 116, fg);
        int len = Util::strLen(title);
        int x = (Const::SCR_W - len) / 2;
        put(x, y, title, BYELLOW);
    }

    // ---- Input ----
    // Returns pointer to static buffer
    const char *input(int x, int y, int maxLen = 30)
    {
        static char buf[256];
        buf[0] = 0;
        int i = 0;
        cursor(true);
        color(BWHITE);
        at(x, y);
        char c;
        while (true)
        {
            c = (char)_getch();
            if (c == 13)
                break;
            if (c == 8 && i > 0)
            {
                buf[--i] = 0;
                at(x + i, y);
                color(BWHITE);
                cout << ' ';
                at(x + i, y);
            }
            else if ((unsigned char)c >= 32 && i < maxLen - 1)
            {
                buf[i++] = c;
                buf[i] = 0;
                color(BWHITE);
                cout << c;
            }
        }
        cursor(false);
        return buf;
    }

    int inputInt(int x, int y, int maxLen = 5)
    {
        return Util::strInt(input(x, y, maxLen));
    }

    void waitKey(const char *m = "  Press any key to return to menu...")
    {
        put(2, 37, m, BYELLOW);
        _getch();
    }

    void clearContent() { fillRect(0, 3, Const::SCR_W, 33); }
}

// ============================================================
//  DATA STRUCTURES
// ============================================================
namespace DS
{

    // --------------------------------------------------------
    //  Classroom  (node for LL and BST)
    // --------------------------------------------------------
    struct Room
    {
        char id[20];
        char name[50];
        int cap;
        bool ac;
        float rating;                      // 1-5
        int clean;                         // 1-10
        bool good;                         // category
        int tt[Const::DAYS][Const::SLOTS]; // 0=free 1=class 2=booked

        Room *llNext; // linked list
        Room *bstL;   // BST left
        Room *bstR;   // BST right
    };

    // --------------------------------------------------------
    //  Booking
    // --------------------------------------------------------
    struct Booking
    {
        int id;
        char roomId[20];
        char userId[20];
        int day, slot;
        bool active;
        Booking *qNext; // queue link
    };

    // --------------------------------------------------------
    //  Action (undo)
    // --------------------------------------------------------
    enum ActType
    {
        ACT_BOOK,
        ACT_CANCEL,
        ACT_EMPTY,
        ACT_RESCHEDULE
    };
    struct Action
    {
        ActType type;
        char roomId[20];
        char userId[20];
        int day, slot;
        int oldDay, oldSlot;
        int bookId;
        int prevState;
    };

    // ============================================================
    //  1. LINKED LIST  — dynamic classroom storage
    // ============================================================
    namespace LL
    {
        struct List
        {
            Room *head;
            int cnt;
        };

        void init(List &l)
        {
            l.head = nullptr;
            l.cnt = 0;
        }

        Room *make(const char *id, const char *name, int cap, bool ac, float rat, int cl)
        {
            Room *r = new Room();
            Util::strCpy(r->id, id);
            Util::strCpy(r->name, name);
            r->cap = cap;
            r->ac = ac;
            r->rating = rat;
            r->clean = cl;
            r->good = (ac && rat >= 3.5f && cl >= 6);
            for (int d = 0; d < Const::DAYS; d++)
                for (int s = 0; s < Const::SLOTS; s++)
                    r->tt[d][s] = 0;
            r->llNext = nullptr;
            r->bstL = r->bstR = nullptr;
            return r;
        }

        bool insert(List &l, Room *r)
        {
            // duplicate check
            Room *c = l.head;
            while (c)
            {
                if (Util::strEq(c->id, r->id))
                    return false;
                c = c->llNext;
            }
            r->llNext = l.head;
            l.head = r;
            l.cnt++;
            return true;
        }

        bool remove(List &l, const char *id)
        {
            Room *c = l.head, *p = nullptr;
            while (c)
            {
                if (Util::strEq(c->id, id))
                {
                    if (p)
                        p->llNext = c->llNext;
                    else
                        l.head = c->llNext;
                    l.cnt--;
                    return true;
                }
                p = c;
                c = c->llNext;
            }
            return false;
        }

        Room *find(List &l, const char *id)
        {
            Room *c = l.head;
            while (c)
            {
                if (Util::strEq(c->id, id))
                    return c;
                c = c->llNext;
            }
            return nullptr;
        }
    }

    // ============================================================
    //  2. BST  — sorted lookup by Room ID
    // ============================================================
    namespace BST
    {
        struct Tree
        {
            Room *root;
        };
        void init(Tree &t) { t.root = nullptr; }

        Room *ins(Room *node, Room *r)
        {
            if (!node)
                return r;
            if (Util::strCmp(r->id, node->id) < 0)
                node->bstL = ins(node->bstL, r);
            else
                node->bstR = ins(node->bstR, r);
            return node;
        }
        void insert(Tree &t, Room *r)
        {
            r->bstL = r->bstR = nullptr;
            t.root = ins(t.root, r);
        }

        Room *search(Room *node, const char *id)
        {
            if (!node)
                return nullptr;
            int c = Util::strCmp(id, node->id);
            if (c == 0)
                return node;
            return c < 0 ? search(node->bstL, id) : search(node->bstR, id);
        }
        Room *search(Tree &t, const char *id) { return search(t.root, id); }

        // Inorder fill
        int inorder(Room *node, Room **arr, int i)
        {
            if (!node)
                return i;
            i = inorder(node->bstL, arr, i);
            arr[i++] = node;
            i = inorder(node->bstR, arr, i);
            return i;
        }
        int sorted(Tree &t, Room **arr) { return inorder(t.root, arr, 0); }
    }

    // ============================================================
    //  3. HASH TABLE  — O(1) room lookup by ID
    // ============================================================
    namespace HT
    {
        struct Entry
        {
            char key[20];
            Room *room;
            Entry *next;
        };
        struct Table
        {
            Entry *b[Const::HASH_SIZE];
        };

        void init(Table &t)
        {
            for (int i = 0; i < Const::HASH_SIZE; i++)
                t.b[i] = nullptr;
        }

        int hash(const char *k)
        {
            unsigned h = 5381;
            while (*k)
                h = (h << 5) + h + (unsigned char)*k++;
            return (int)(h % Const::HASH_SIZE);
        }

        void put(Table &t, const char *k, Room *r)
        {
            int h = hash(k);
            Entry *e = new Entry();
            Util::strCpy(e->key, k);
            e->room = r;
            e->next = t.b[h];
            t.b[h] = e;
        }

        Room *get(Table &t, const char *k)
        {
            Entry *e = t.b[hash(k)];
            while (e)
            {
                if (Util::strEq(e->key, k))
                    return e->room;
                e = e->next;
            }
            return nullptr;
        }

        bool del(Table &t, const char *k)
        {
            int h = hash(k);
            Entry *e = t.b[h], *p = nullptr;
            while (e)
            {
                if (Util::strEq(e->key, k))
                {
                    if (p)
                        p->next = e->next;
                    else
                        t.b[h] = e->next;
                    delete e;
                    return true;
                }
                p = e;
                e = e->next;
            }
            return false;
        }
    }

    // ============================================================
    //  4. QUEUE (FIFO)  — booking requests
    // ============================================================
    namespace Q
    {
        struct Node
        {
            Booking *b;
            Node *next;
        };
        struct Queue
        {
            Node *front;
            Node *rear;
            int cnt;
        };

        void init(Queue &q)
        {
            q.front = q.rear = nullptr;
            q.cnt = 0;
        }

        void push(Queue &q, Booking *b)
        {
            Node *n = new Node();
            n->b = b;
            n->next = nullptr;
            if (q.rear)
                q.rear->next = n;
            else
                q.front = n;
            q.rear = n;
            q.cnt++;
        }

        Booking *pop(Queue &q)
        {
            if (!q.front)
                return nullptr;
            Node *n = q.front;
            Booking *b = n->b;
            q.front = q.front->next;
            if (!q.front)
                q.rear = nullptr;
            delete n;
            q.cnt--;
            return b;
        }

        bool removeId(Queue &q, int id)
        {
            Node *c = q.front, *p = nullptr;
            while (c)
            {
                if (c->b->id == id)
                {
                    if (p)
                        p->next = c->next;
                    else
                        q.front = c->next;
                    if (!c->next)
                        q.rear = p;
                    delete c;
                    q.cnt--;
                    return true;
                }
                p = c;
                c = c->next;
            }
            return false;
        }

        bool empty(Queue &q) { return q.front == nullptr; }
    }

    // ============================================================
    //  5. STACK  — undo history
    // ============================================================
    namespace Stk
    {
        struct Stack
        {
            Action data[Const::MAX_UNDO];
            int top;
        };

        void init(Stack &s) { s.top = -1; }

        bool push(Stack &s, Action a)
        {
            if (s.top == Const::MAX_UNDO - 1)
            {
                // Shift oldest off bottom
                for (int i = 0; i < Const::MAX_UNDO - 1; i++)
                    s.data[i] = s.data[i + 1];
                s.data[Const::MAX_UNDO - 1] = a;
                return true;
            }
            s.data[++s.top] = a;
            return true;
        }

        bool pop(Stack &s, Action &a)
        {
            if (s.top < 0)
                return false;
            a = s.data[s.top--];
            return true;
        }
        bool peek(Stack &s, Action &a)
        {
            if (s.top < 0)
                return false;
            a = s.data[s.top];
            return true;
        }
        bool empty(Stack &s) { return s.top < 0; }
        int size(Stack &s) { return s.top + 1; }
    }

    // ============================================================
    //  6. PRIORITY QUEUE (Max-Heap)  — room recommendations
    // ============================================================
    namespace PQ
    {
        struct Pair
        {
            Room *r;
            float score;
        };
        struct Heap
        {
            Pair data[Const::MAX_ROOMS];
            int sz;
        };

        void init(Heap &h) { h.sz = 0; }

        float score(Room *r)
        {
            float s = r->rating * 2.0f;
            if (r->ac)
                s += 3.0f;
            if (r->good)
                s += 5.0f;
            s += r->clean * 0.5f;
            return s;
        }

        void swp(Heap &h, int a, int b)
        {
            Pair t = h.data[a];
            h.data[a] = h.data[b];
            h.data[b] = t;
        }

        void up(Heap &h, int i)
        {
            while (i > 0)
            {
                int p = (i - 1) / 2;
                if (h.data[p].score < h.data[i].score)
                {
                    swp(h, p, i);
                    i = p;
                }
                else
                    break;
            }
        }

        void dn(Heap &h, int i)
        {
            while (true)
            {
                int lg = i, l = 2 * i + 1, r = 2 * i + 2;
                if (l < h.sz && h.data[l].score > h.data[lg].score)
                    lg = l;
                if (r < h.sz && h.data[r].score > h.data[lg].score)
                    lg = r;
                if (lg != i)
                {
                    swp(h, lg, i);
                    i = lg;
                }
                else
                    break;
            }
        }

        void insert(Heap &h, Room *r)
        {
            if (h.sz >= Const::MAX_ROOMS)
                return;
            h.data[h.sz] = {r, score(r)};
            up(h, h.sz++);
        }

        Room *extractMax(Heap &h)
        {
            if (!h.sz)
                return nullptr;
            Room *top = h.data[0].r;
            h.data[0] = h.data[--h.sz];
            dn(h, 0);
            return top;
        }

        bool empty(Heap &h) { return h.sz == 0; }
    }

} // namespace DS

// ============================================================
//  SYSTEM  — business logic layer
// ============================================================
namespace Sys
{
    using namespace DS;

    // Global state
    LL::List rooms;
    BST::Tree bst;
    HT::Table ht;
    Q::Queue bq;
    Stk::Stack undoStk;

    Booking *allBk[Const::MAX_BOOKINGS];
    int bkCnt = 0;
    int nextBid = 1;

    const char *DAY_NAME[5] = {"Monday", "Tuesday", "Wednesday", "Thursday", "Friday"};
    const char *SLOT_NAME[8] = {"08:00-09:00", "09:00-10:00", "10:00-11:00", "11:00-12:00",
                                "12:00-01:00", "01:00-02:00", "02:00-03:00", "03:00-04:00"};

    void init()
    {
        LL::init(rooms);
        BST::init(bst);
        HT::init(ht);
        Q::init(bq);
        Stk::init(undoStk);
        bkCnt = 0;
        nextBid = 1;
    }

    // ---- Helpers ----
    Room *getRoom(const char *id) { return HT::get(ht, id); }

    // ---- Add Classroom ----
    bool addRoom(const char *id, const char *name, int cap, bool ac, float rat, int cl)
    {
        Room *r = LL::make(id, name, cap, ac, rat, cl);
        if (!LL::insert(rooms, r))
        {
            delete r;
            return false;
        }
        BST::insert(bst, r);
        HT::put(ht, id, r);
        return true;
    }

    // ---- Remove Classroom ----
    bool removeRoom(const char *id)
    {
        if (!HT::get(ht, id))
            return false;
        HT::del(ht, id);
        LL::remove(rooms, id);
        return true;
    }

    // ---- Check Availability ----
    bool available(const char *id, int d, int s)
    {
        Room *r = getRoom(id);
        return r && r->tt[d][s] == 0;
    }

    // ---- Book Room ----
    int bookRoom(const char *rid, const char *uid, int d, int s)
    {
        Room *r = getRoom(rid);
        if (!r || r->tt[d][s] != 0)
            return -1;

        Booking *b = new Booking();
        b->id = nextBid++;
        Util::strCpy(b->roomId, rid);
        Util::strCpy(b->userId, uid);
        b->day = d;
        b->slot = s;
        b->active = true;
        b->qNext = nullptr;

        r->tt[d][s] = 2;
        Q::push(bq, b);
        if (bkCnt < Const::MAX_BOOKINGS)
            allBk[bkCnt++] = b;

        Action a;
        a.type = ACT_BOOK;
        Util::strCpy(a.roomId, rid);
        Util::strCpy(a.userId, uid);
        a.day = d;
        a.slot = s;
        a.bookId = b->id;
        a.prevState = 0;
        Stk::push(undoStk, a);
        return b->id;
    }

    // ---- Cancel Booking ----
    bool cancelBooking(int bid)
    {
        for (int i = 0; i < bkCnt; i++)
        {
            if (allBk[i] && allBk[i]->id == bid && allBk[i]->active)
            {
                Booking *b = allBk[i];
                Room *r = getRoom(b->roomId);
                if (r)
                    r->tt[b->day][b->slot] = 0;
                b->active = false;
                Q::removeId(bq, bid);

                Action a;
                a.type = ACT_CANCEL;
                Util::strCpy(a.roomId, b->roomId);
                Util::strCpy(a.userId, b->userId);
                a.day = b->day;
                a.slot = b->slot;
                a.bookId = bid;
                a.prevState = 2;
                Stk::push(undoStk, a);
                return true;
            }
        }
        return false;
    }

    // ---- Mark Class Empty ----
    bool markEmpty(const char *rid, int d, int s)
    {
        Room *r = getRoom(rid);
        if (!r)
            return false;
        int prev = r->tt[d][s];
        r->tt[d][s] = 0;

        Action a;
        a.type = ACT_EMPTY;
        Util::strCpy(a.roomId, rid);
        a.day = d;
        a.slot = s;
        a.prevState = prev;
        Stk::push(undoStk, a);
        return true;
    }

    // ---- Reschedule ----
    bool reschedule(const char *rid, int od, int os, int nd, int ns)
    {
        Room *r = getRoom(rid);
        if (!r || r->tt[nd][ns] != 0)
            return false;
        int prev = r->tt[od][os];
        r->tt[od][os] = 0;
        r->tt[nd][ns] = 1;

        Action a;
        a.type = ACT_RESCHEDULE;
        Util::strCpy(a.roomId, rid);
        a.day = nd;
        a.slot = ns;
        a.oldDay = od;
        a.oldSlot = os;
        a.prevState = prev;
        Stk::push(undoStk, a);
        return true;
    }

    // ---- Undo ----
    bool undo()
    {
        Action a;
        if (!Stk::pop(undoStk, a))
            return false;
        Room *r = getRoom(a.roomId);
        if (!r)
            return false;

        if (a.type == ACT_BOOK)
        {
            r->tt[a.day][a.slot] = 0;
            for (int i = 0; i < bkCnt; i++)
                if (allBk[i] && allBk[i]->id == a.bookId)
                {
                    allBk[i]->active = false;
                    break;
                }
        }
        else if (a.type == ACT_CANCEL)
        {
            r->tt[a.day][a.slot] = a.prevState;
            for (int i = 0; i < bkCnt; i++)
                if (allBk[i] && allBk[i]->id == a.bookId)
                {
                    allBk[i]->active = true;
                    break;
                }
        }
        else if (a.type == ACT_EMPTY)
        {
            r->tt[a.day][a.slot] = a.prevState;
        }
        else if (a.type == ACT_RESCHEDULE)
        {
            r->tt[a.day][a.slot] = 0;
            r->tt[a.oldDay][a.oldSlot] = a.prevState;
        }
        return true;
    }

    // ---- Recommend (Priority Queue) ----
    int recommend(int d, int s, Room **out, int maxOut)
    {
        PQ::Heap h;
        PQ::init(h);
        Room *c = rooms.head;
        while (c)
        {
            if (c->tt[d][s] == 0)
                PQ::insert(h, c);
            c = c->llNext;
        }
        int cnt = 0;
        while (!PQ::empty(h) && cnt < maxOut)
            out[cnt++] = PQ::extractMax(h);
        return cnt;
    }

    // ---- Find Free Rooms (2-D array scan) ----
    int freeRooms(int d, int s, Room **out, int maxOut)
    {
        int cnt = 0;
        Room *c = rooms.head;
        while (c && cnt < maxOut)
        {
            if (c->tt[d][s] == 0)
                out[cnt++] = c;
            c = c->llNext;
        }
        return cnt;
    }

    // ---- Categorize Room ----
    bool categorize(const char *rid, bool ac, float rat, int cl)
    {
        Room *r = getRoom(rid);
        if (!r)
            return false;
        r->ac = ac;
        r->rating = rat;
        r->clean = cl;
        r->good = (ac && rat >= 3.5f && cl >= 6);
        return true;
    }

    // ---- Load sample data ----
    void loadSample()
    {
        addRoom("R101", "Lecture Hall A", 120, true, 4.5f, 9);
        addRoom("R102", "Lecture Hall B", 100, true, 4.0f, 8);
        addRoom("R103", "Seminar Room", 40, false, 3.0f, 5);
        addRoom("R104", "Computer Lab", 60, true, 4.8f, 10);
        addRoom("R105", "Tutorial Room", 30, false, 2.5f, 4);
        addRoom("R106", "Conference Hall", 80, true, 4.2f, 8);
        addRoom("R107", "Study Hall", 50, false, 3.8f, 7);
        addRoom("R108", "Physics Lab", 45, true, 4.1f, 7);

        Room *r;
        r = getRoom("R101");
        if (r)
        {
            r->tt[0][0] = 1;
            r->tt[0][2] = 1;
            r->tt[2][4] = 1;
        }
        r = getRoom("R102");
        if (r)
        {
            r->tt[1][1] = 1;
            r->tt[3][3] = 1;
        }
        r = getRoom("R104");
        if (r)
        {
            r->tt[0][1] = 1;
            r->tt[4][5] = 1;
        }
        r = getRoom("R106");
        if (r)
        {
            r->tt[2][2] = 1;
            r->tt[4][6] = 1;
        }
    }
}

// ============================================================
//  SCREENS  — all UI screens
// ============================================================
namespace Scr
{
    using namespace GUI;
    using namespace Sys;

    // --------------------------------------------------------
    //  Helper: pick day & slot interactively
    // --------------------------------------------------------
    void pickDaySlot(int &day, int &slot, int baseY)
    {
        put(4, baseY, "Select Day:", BCYAN);
        put(60, baseY, "Select Slot:", BCYAN);
        for (int i = 0; i < 5; i++)
        {
            char opt[40] = "  ";
            char n[4];
            Util::intStr(i + 1, n);
            Util::strCat(opt, n);
            Util::strCat(opt, ". ");
            Util::strCat(opt, DAY_NAME[i]);
            put(4, baseY + 1 + i, opt, BWHITE);
        }
        for (int i = 0; i < 8; i++)
        {
            char opt[40] = "  ";
            char n[4];
            Util::intStr(i + 1, n);
            Util::strCat(opt, n);
            Util::strCat(opt, ". ");
            Util::strCat(opt, SLOT_NAME[i]);
            put(60, baseY + 1 + i, opt, BWHITE);
        }
        put(4, baseY + 7, "Day  (1-5): ", BCYAN);
        day = inputInt(16, baseY + 7, 2) - 1;
        put(4, baseY + 8, "Slot (1-8): ", BCYAN);
        slot = inputInt(16, baseY + 8, 2) - 1;
        if (day < 0 || day > 4)
            day = 0;
        if (slot < 0 || slot > 7)
            slot = 0;
    }

    // --------------------------------------------------------
    //  Room table row
    // --------------------------------------------------------
    void roomRow(int row, int idx, Room *r)
    {
        char tmp[30];
        color(r->good ? BGREEN : BRED);
        Util::intStr(idx, tmp);
        at(2, row);
        cout << " " << tmp;
        at(8, row);
        cout << r->id;
        at(18, row);
        cout << r->name;
        at(44, row);
        Util::intStr(r->cap, tmp);
        cout << tmp;
        at(52, row);
        cout << (r->ac ? "Yes" : "No ");
        at(58, row);
        Util::intStr((int)r->rating, tmp);
        cout << tmp;
        cout << ".";
        Util::intStr((int)((r->rating - (int)r->rating) * 10), tmp);
        cout << tmp;
        at(65, row);
        Util::intStr(r->clean, tmp);
        cout << tmp;
        at(72, row);
        color(r->good ? BGREEN : BRED);
        cout << (r->good ? "[  GOOD  ]" : "[  BAD   ]");
    }

    // ============================================================
    //  MAIN MENU
    // ============================================================
    int mainMenu()
    {
        banner();
        dbox(30, 4, 60, 32, BCYAN);
        putC(30, 6, 60, "[ MAIN MENU ]", BYELLOW);
        hline(31, 7, 58, BCYAN);

        struct
        {
            const char *txt;
            Color c;
        } opts[] = {
            {"  1.  Add Classroom", BGREEN},
            {"  2.  Remove Classroom", BRED},
            {"  3.  View All Classrooms", BCYAN},
            {"  4.  Book a Room", BYELLOW},
            {"  5.  Cancel Booking", BMAGENTA},
            {"  6.  Mark Class as Empty", BWHITE},
            {"  7.  Undo Last Action", CYAN},
            {"  8.  Reschedule Class", GREEN},
            {"  9.  Recommend Rooms", YELLOW},
            {" 10.  Find Free Rooms", MAGENTA},
            {" 11.  Display Room Info", BCYAN},
            {" 12.  View Booking Queue", BGREEN},
            {" 13.  Categorize Room", BYELLOW},
            {" 14.  Load Sample Data", WHITE},
            {"  0.  Exit", BRED},
        };
        int n = 15;
        for (int i = 0; i < n; i++)
            put(38, 9 + i, opts[i].txt, opts[i].c);

        hline(31, 25, 58, BCYAN);
        put(38, 26, "  DSA driven — every structure manual  ", CYAN);
        hline(31, 27, 58, BCYAN);
        put(38, 29, "  Enter choice: ", BWHITE);
        cursor(true);
        int ch = inputInt(54, 29, 3);
        cursor(false);
        return ch;
    }

    // ============================================================
    //  1. Add Classroom
    // ============================================================
    void addClassroom()
    {
        banner();
        clearContent();
        dbox(20, 4, 80, 22, BGREEN);
        putC(20, 6, 80, "[ ADD NEW CLASSROOM ]", BYELLOW);
        hline(21, 8, 78, BGREEN);

        char id[20], name[50], acIn[5], rStr[10];
        int cap, cl;

        put(28, 10, "Room ID          : ", BCYAN);
        Util::strCpy(id, input(47, 10, 15));
        put(28, 12, "Room Name        : ", BCYAN);
        Util::strCpy(name, input(47, 12, 40));
        put(28, 14, "Capacity         : ", BCYAN);
        cap = inputInt(47, 14, 5);
        put(28, 16, "Has AC?  (1=Yes) : ", BCYAN);
        Util::strCpy(acIn, input(47, 16, 3));
        put(28, 18, "Rating   (1.0-5.0): ", BCYAN);
        Util::strCpy(rStr, input(48, 18, 6));
        put(28, 20, "Clean Score (1-10): ", BCYAN);
        cl = inputInt(48, 20, 3);

        if (!Util::strLen(id))
        {
            errMsg("Room ID cannot be empty!");
            waitKey();
            return;
        }
        bool ac = (acIn[0] == '1');
        float rat = Util::strFloat(rStr);
        if (rat < 1.0f)
            rat = 1.0f;
        if (rat > 5.0f)
            rat = 5.0f;
        if (cl < 1)
            cl = 1;
        if (cl > 10)
            cl = 10;

        if (addRoom(id, name, cap, ac, rat, cl))
            okMsg("Classroom added successfully!");
        else
            errMsg("Duplicate Room ID — classroom already exists!");
        waitKey();
    }

    // ============================================================
    //  2. Remove Classroom
    // ============================================================
    void removeClassroom()
    {
        banner();
        clearContent();
        dbox(30, 10, 60, 10, BRED);
        putC(30, 12, 60, "[ REMOVE CLASSROOM ]", BYELLOW);
        hline(31, 14, 58, BRED);
        put(36, 16, "Enter Room ID: ", BCYAN);
        char id[20];
        Util::strCpy(id, input(51, 16, 15));

        if (removeRoom(id))
            okMsg("Classroom removed from Linked List and Hash Table.");
        else
            errMsg("Room not found in system!");
        waitKey();
    }

    // ============================================================
    //  3. View All Classrooms  (LL traversal)
    // ============================================================
    void viewAll()
    {
        banner();
        clearContent();
        section(4, " ALL CLASSROOMS  —  Linked List Traversal  ");
        put(2, 5, "  #   Room ID    Name                         Cap   AC   Rating  Clean  Category", BYELLOW);
        hline(2, 6, 116, BCYAN);

        Room *c = rooms.head;
        int i = 1, row = 7;
        while (c && row < 35)
        {
            roomRow(row, i, c);
            c = c->llNext;
            i++;
            row++;
        }

        if (!rooms.cnt)
            putC(0, 18, Const::SCR_W, "No classrooms yet — use option 14 to load samples.", BRED);

        char cnt[10], msg[60] = "Total rooms in Linked List: ";
        Util::intStr(rooms.cnt, cnt);
        Util::strCat(msg, cnt);
        put(2, 35, msg, BCYAN);
        waitKey();
    }

    // ============================================================
    //  4. Book a Room
    // ============================================================
    void bookRoom()
    {
        banner();
        clearContent();
        dbox(15, 4, 90, 26, BYELLOW);
        putC(15, 6, 90, "[ BOOK A ROOM ]", BYELLOW);
        hline(16, 8, 88, BYELLOW);

        char rid[20], uid[20];
        put(25, 10, "Room ID : ", BCYAN);
        Util::strCpy(rid, input(35, 10, 15));
        put(25, 12, "User ID : ", BCYAN);
        Util::strCpy(uid, input(35, 12, 15));

        int day, slot;
        pickDaySlot(day, slot, 14);

        if (!available(rid, day, slot))
        {
            errMsg("Slot not available — already occupied or booked!");
            waitKey();
            return;
        }
        int bid = bookRoom(rid, uid, day, slot);
        if (bid > 0)
        {
            char msg[80] = "Booking confirmed!  Booking ID: ";
            char tmp[10];
            Util::intStr(bid, tmp);
            Util::strCat(msg, tmp);
            okMsg(msg);
        }
        else
            errMsg("Booking failed — room not found!");
        waitKey();
    }

    // ============================================================
    //  5. Cancel Booking
    // ============================================================
    void cancelBooking()
    {
        banner();
        clearContent();
        dbox(30, 10, 60, 10, BMAGENTA);
        putC(30, 12, 60, "[ CANCEL BOOKING ]", BYELLOW);
        hline(31, 14, 58, BMAGENTA);
        put(36, 16, "Booking ID: ", BCYAN);
        int bid = inputInt(48, 16, 6);

        if (cancelBooking(bid))
            okMsg("Booking cancelled. Slot freed. Action pushed to undo stack.");
        else
            errMsg("Booking ID not found or already cancelled!");
        waitKey();
    }

    // ============================================================
    //  6. Mark Class Empty
    // ============================================================
    void markEmpty()
    {
        banner();
        clearContent();
        dbox(15, 4, 90, 26, BMAGENTA);
        putC(15, 6, 90, "[ MARK CLASS AS EMPTY ]", BYELLOW);
        hline(16, 8, 88, BMAGENTA);

        char rid[20];
        put(25, 10, "Room ID : ", BCYAN);
        Util::strCpy(rid, input(35, 10, 15));

        int day, slot;
        pickDaySlot(day, slot, 12);

        if (markEmpty(rid, day, slot))
            okMsg("Slot freed — room now available for booking.");
        else
            errMsg("Room not found!");
        waitKey();
    }

    // ============================================================
    //  7. Undo Last Action
    // ============================================================
    void undoAction()
    {
        banner();
        clearContent();
        dbox(25, 8, 70, 16, BCYAN);
        putC(25, 10, 70, "[ UNDO LAST ACTION ]", BYELLOW);
        hline(26, 12, 68, BCYAN);

        Action a;
        if (Stk::peek(undoStk, a))
        {
            const char *tnames[] = {"BOOK", "CANCEL", "MARK EMPTY", "RESCHEDULE"};
            put(30, 14, "Action type : ", BWHITE);
            put(44, 14, tnames[a.type], BYELLOW);
            put(30, 15, "Room ID     : ", BWHITE);
            put(44, 15, a.roomId, BGREEN);
            put(30, 16, "Day / Slot  : ", BWHITE);
            at(44, 16);
            color(BGREEN);
            cout << DAY_NAME[a.day] << " / " << SLOT_NAME[a.slot];
            put(30, 18, "Undo stack depth: ", BWHITE);
            char tmp[10];
            Util::intStr(Stk::size(undoStk), tmp);
            put(48, 18, tmp, BYELLOW);
        }
        else
        {
            putC(25, 18, 70, "Stack is empty — nothing to undo.", BRED);
            waitKey();
            return;
        }

        if (undo())
            okMsg("Last action undone successfully!");
        else
            errMsg("Undo failed — stack is empty!");
        waitKey();
    }

    // ============================================================
    //  8. Reschedule Class
    // ============================================================
    void rescheduleClass()
    {
        banner();
        clearContent();
        dbox(10, 4, 100, 26, BGREEN);
        putC(10, 6, 100, "[ RESCHEDULE CLASS ]", BYELLOW);
        hline(11, 8, 98, BGREEN);

        char rid[20];
        put(20, 10, "Room ID: ", BCYAN);
        Util::strCpy(rid, input(29, 10, 15));

        put(20, 12, "--- OLD SLOT ---", BYELLOW);
        put(20, 13, "Old Day  (1-5): ", BCYAN);
        int od = inputInt(36, 13, 2) - 1;
        put(20, 14, "Old Slot (1-8): ", BCYAN);
        int os = inputInt(36, 14, 2) - 1;

        put(70, 12, "--- NEW SLOT ---", BYELLOW);
        put(70, 13, "New Day  (1-5): ", BCYAN);
        int nd = inputInt(86, 13, 2) - 1;
        put(70, 14, "New Slot (1-8): ", BCYAN);
        int ns = inputInt(86, 14, 2) - 1;

        if (od < 0 || od > 4 || os < 0 || os > 7 || nd < 0 || nd > 4 || ns < 0 || ns > 7)
        {
            errMsg("Invalid day or slot values!");
            waitKey();
            return;
        }
        if (reschedule(rid, od, os, nd, ns))
            okMsg("Class rescheduled successfully!");
        else
            errMsg("Failed — new slot occupied, or room not found!");
        waitKey();
    }

    // ============================================================
    //  9. Recommend Rooms  (Priority Queue / Max-Heap)
    // ============================================================
    void recommendRooms()
    {
        banner();
        clearContent();
        section(4, " ROOM RECOMMENDATIONS  —  Priority Queue (Max-Heap)  ");

        put(4, 6, "Day  (1-5): ", BCYAN);
        int day = inputInt(16, 6, 2) - 1;
        put(4, 7, "Slot (1-8): ", BCYAN);
        int slot = inputInt(16, 7, 2) - 1;
        if (day < 0 || day > 4 || slot < 0 || slot > 7)
        {
            errMsg("Invalid input!");
            waitKey();
            return;
        }

        Room *res[Const::MAX_ROOMS];
        int cnt = recommend(day, slot, res, 12);

        hline(2, 9, 116, BCYAN);
        put(2, 10, " Rank  Room ID    Name                       Cap   AC    Rating  Clean  Score   Category", BYELLOW);
        hline(2, 11, 116, BCYAN);

        if (!cnt)
        {
            putC(0, 20, Const::SCR_W, "No rooms available for this slot.", BRED);
        }
        else
        {
            for (int i = 0; i < cnt; i++)
            {
                Room *r = res[i];
                color(r->good ? BGREEN : BRED);
                char tmp[20];
                at(2, 12 + i);
                Util::intStr(i + 1, tmp);
                cout << "  #" << tmp;
                at(10, 12 + i);
                cout << r->id;
                at(20, 12 + i);
                cout << r->name;
                at(47, 12 + i);
                Util::intStr(r->cap, tmp);
                cout << tmp;
                at(53, 12 + i);
                cout << (r->ac ? "Yes" : "No ");
                at(59, 12 + i);
                Util::intStr((int)r->rating, tmp);
                cout << tmp << ".";
                Util::intStr((int)((r->rating - (int)r->rating) * 10), tmp);
                cout << tmp;
                at(67, 12 + i);
                Util::intStr(r->clean, tmp);
                cout << tmp;
                at(73, 12 + i);
                float sc = PQ::score(r);
                Util::intStr((int)sc, tmp);
                cout << tmp;
                at(80, 12 + i);
                cout << (r->good ? "[  GOOD  ]" : "[  BAD   ]");
            }
        }

        char info[80] = "Queried: ";
        Util::strCat(info, DAY_NAME[day]);
        Util::strCat(info, "  /  ");
        Util::strCat(info, SLOT_NAME[slot]);
        put(2, 35, info, BCYAN);
        waitKey();
    }

    // ============================================================
    //  10. Find Free Rooms  (2-D timetable scan)
    // ============================================================
    void findFree()
    {
        banner();
        clearContent();
        section(4, " FREE ROOMS  —  2-D Timetable Scan  ");

        put(4, 6, "Day  (1-5): ", BCYAN);
        int day = inputInt(16, 6, 2) - 1;
        put(4, 7, "Slot (1-8): ", BCYAN);
        int slot = inputInt(16, 7, 2) - 1;
        if (day < 0 || day > 4 || slot < 0 || slot > 7)
        {
            errMsg("Invalid input!");
            waitKey();
            return;
        }

        Room *res[Const::MAX_ROOMS];
        int cnt = freeRooms(day, slot, res, Const::MAX_ROOMS);

        hline(2, 9, 116, BCYAN);
        put(2, 10, "  #   Room ID    Name                         Cap   AC     Category", BYELLOW);
        hline(2, 11, 116, BCYAN);

        if (!cnt)
        {
            putC(0, 20, Const::SCR_W, "No free rooms for this slot.", BRED);
        }
        else
        {
            for (int i = 0; i < cnt && i < 22; i++)
            {
                Room *r = res[i];
                char tmp[20];
                color(r->good ? BGREEN : BWHITE);
                at(2, 12 + i);
                Util::intStr(i + 1, tmp);
                cout << "  " << tmp;
                at(8, 12 + i);
                cout << r->id;
                at(18, 12 + i);
                cout << r->name;
                at(46, 12 + i);
                Util::intStr(r->cap, tmp);
                cout << tmp;
                at(53, 12 + i);
                cout << (r->ac ? "Yes" : "No ");
                at(60, 12 + i);
                cout << (r->good ? "[  GOOD  ]" : "[  BAD   ]");
            }
        }

        char msg[50] = "Free rooms found: ";
        char tmp[10];
        Util::intStr(cnt, tmp);
        Util::strCat(msg, tmp);
        put(2, 35, msg, BCYAN);
        waitKey();
    }

    // ============================================================
    //  11. Display Room Info + Timetable
    // ============================================================
    void roomInfo()
    {
        banner();
        clearContent();
        section(4, " ROOM INFORMATION & WEEKLY TIMETABLE  —  Hash Table Lookup  ");

        put(4, 6, "Room ID: ", BCYAN);
        char rid[20];
        Util::strCpy(rid, input(13, 6, 15));

        Room *r = getRoom(rid);
        if (!r)
        {
            errMsg("Room not found in Hash Table!");
            waitKey();
            return;
        }

        // Details panel
        box(2, 8, 48, 8, BCYAN);
        put(4, 9, "Room Details", BYELLOW);
        hline(3, 10, 46, BCYAN);
        char tmp[30];
        put(4, 11, "ID       : ", BWHITE);
        put(15, 11, r->id, BGREEN);
        put(4, 12, "Name     : ", BWHITE);
        put(15, 12, r->name, BGREEN);
        put(4, 13, "Capacity : ", BWHITE);
        Util::intStr(r->cap, tmp);
        put(15, 13, tmp, BGREEN);
        put(4, 14, "AC       : ", BWHITE);
        put(15, 14, r->ac ? "Yes" : "No", BGREEN);
        put(4, 15, "Category : ", BWHITE);
        put(15, 15, r->good ? "GOOD" : "BAD", r->good ? BGREEN : BRED);

        // Score panel
        box(52, 8, 64, 8, BYELLOW);
        put(54, 9, "Scores & Rating", BYELLOW);
        hline(53, 10, 62, BYELLOW);
        put(54, 11, "Rating     : ", BWHITE);
        Util::intStr((int)r->rating, tmp);
        at(67, 11);
        color(BGREEN);
        cout << tmp << ".";
        Util::intStr((int)((r->rating - (int)r->rating) * 10), tmp);
        cout << tmp << "/5.0";
        put(54, 12, "Clean Score: ", BWHITE);
        Util::intStr(r->clean, tmp);
        put(67, 12, tmp, BGREEN);
        put(54, 13, "PQ Score   : ", BWHITE);
        Util::intStr((int)PQ::score(r), tmp);
        put(67, 13, tmp, BYELLOW);

        // Timetable
        box(2, 17, 116, 18, BYELLOW);
        put(4, 18, "WEEKLY TIMETABLE  (0=Free  1=Class  2=Booked)", BYELLOW);
        hline(3, 19, 114, BYELLOW);

        // Header row
        put(4, 20, "Slot       ", BCYAN);
        for (int d = 0; d < Const::DAYS; d++)
        {
            at(15 + d * 20, 20);
            color(BCYAN);
            cout << DAY_NAME[d];
        }

        for (int s = 0; s < Const::SLOTS; s++)
        {
            color(BWHITE);
            at(4, 21 + s);
            cout << SLOT_NAME[s];
            for (int d = 0; d < Const::DAYS; d++)
            {
                int v = r->tt[d][s];
                at(15 + d * 20, 21 + s);
                if (v == 0)
                {
                    color(BGREEN);
                    cout << "[  FREE   ]";
                }
                else if (v == 1)
                {
                    color(BRED);
                    cout << "[ OCCUPIED]";
                }
                else
                {
                    color(BYELLOW);
                    cout << "[  BOOKED ]";
                }
            }
        }
        waitKey();
    }

    // ============================================================
    //  12. View Booking Queue
    // ============================================================
    void viewQueue()
    {
        banner();
        clearContent();
        section(4, " BOOKING QUEUE  —  FIFO Order  ");

        put(2, 6, "  #    BkID   Room ID    User ID          Day             Slot            Status", BYELLOW);
        hline(2, 7, 116, BCYAN);

        int row = 8, cnt = 0;
        for (int i = 0; i < bkCnt && row < 35; i++)
        {
            if (!allBk[i])
                continue;
            Booking *b = allBk[i];
            color(b->active ? BGREEN : BRED);
            char tmp[20];
            at(2, row);
            Util::intStr(i + 1, tmp);
            cout << "  " << tmp;
            at(9, row);
            Util::intStr(b->id, tmp);
            cout << tmp;
            at(16, row);
            cout << b->roomId;
            at(27, row);
            cout << b->userId;
            at(44, row);
            cout << DAY_NAME[b->day];
            at(58, row);
            cout << SLOT_NAME[b->slot];
            at(72, row);
            cout << (b->active ? "[  ACTIVE   ]" : "[ CANCELLED ]");
            row++;
            cnt++;
        }

        if (!cnt)
            putC(0, 18, Const::SCR_W, "No bookings in the system yet.", BRED);

        char msg[60] = "Total booking records: ";
        char tmp[10];
        Util::intStr(cnt, tmp);
        Util::strCat(msg, tmp);
        put(2, 35, msg, BCYAN);
        waitKey();
    }

    // ============================================================
    //  13. Categorize Room
    // ============================================================
    void categorizeRoom()
    {
        banner();
        clearContent();
        dbox(20, 5, 80, 20, BMAGENTA);
        putC(20, 7, 80, "[ CATEGORIZE / UPDATE ROOM ]", BYELLOW);
        hline(21, 9, 78, BMAGENTA);

        char rid[20];
        put(28, 11, "Room ID          : ", BCYAN);
        Util::strCpy(rid, input(47, 11, 15));

        Room *r = getRoom(rid);
        if (!r)
        {
            errMsg("Room not found!");
            waitKey();
            return;
        }

        char acIn[5], rStr[10];
        int cl;
        put(28, 13, "Has AC?  (1=Yes) : ", BCYAN);
        Util::strCpy(acIn, input(47, 13, 3));
        put(28, 15, "Rating   (1.0-5.0): ", BCYAN);
        Util::strCpy(rStr, input(48, 15, 6));
        put(28, 17, "Clean Score (1-10): ", BCYAN);
        cl = inputInt(48, 17, 3);

        float rat = Util::strFloat(rStr);
        if (rat < 1.0f)
            rat = 1.0f;
        if (rat > 5.0f)
            rat = 5.0f;
        if (cl < 1)
            cl = 1;
        if (cl > 10)
            cl = 10;

        categorize(rid, acIn[0] == '1', rat, cl);

        char msg[60] = "Room re-categorized as: ";
        Util::strCat(msg, r->good ? "GOOD" : "BAD");
        okMsg(msg);
        waitKey();
    }

    // ============================================================
    //  SPLASH SCREEN
    // ============================================================
    void splash()
    {
        cls();
        color(BCYAN);
        // Title box
        at(10, 2);
        for (int i = 0; i < 100; i++)
            cout << (char)205;
        at(10, 14);
        for (int i = 0; i < 100; i++)
            cout << (char)205;
        at(10, 2);
        cout << (char)201;
        at(109, 2);
        cout << (char)187;
        at(10, 14);
        cout << (char)200;
        at(109, 14);
        cout << (char)188;
        for (int r = 3; r < 14; r++)
        {
            at(10, r);
            cout << (char)186;
            at(109, r);
            cout << (char)186;
        }

        // ASCII banner
        const char *art[] = {
            " ___   ____    _    ____  ____  ",
            "/ ___| / ___|  / \\  | __ )/ ___| ",
            "\\___ \\| |     / _ \\ |  _ \\___ \\ ",
            " ___) | |___ / ___ \\| |_) |__) |",
            "|____/ \\____/_/   \\_\\____/____/ "};
        for (int i = 0; i < 5; i++)
            putC(10, 4 + i, 100, art[i], BCYAN);

        putC(10, 10, 100, "Smart Classroom Availability & Booking System", BYELLOW);
        putC(10, 11, 100, "Complete DSA Implementation — No STL Containers", BWHITE);
        putC(10, 12, 100, "Linked List  |  BST  |  Hash Table  |  Queue  |  Stack  |  Priority Queue  |  2-D Array", CYAN);

        // DSA legend
        color(BGREEN);
        at(15, 16);
        cout << (char)204;
        for (int i = 0; i < 89; i++)
            cout << (char)205;
        cout << (char)185;
        at(15, 18);
        cout << (char)200;
        for (int i = 0; i < 89; i++)
            cout << (char)205;
        cout << (char)188;
        for (int r = 16; r <= 18; r++)
        {
            at(15, r);
            cout << (char)186;
            at(104, r);
            cout << (char)186;
        }
        putC(15, 17, 90, "  Linked List  ->  BST  ->  Hash Table  ->  Queue  ->  Stack  ->  Priority Queue  ", BGREEN);

        putC(0, 22, Const::SCR_W, "[ Press any key to enter the system ]", BYELLOW);

        color(BWHITE);
        at(0, 38);
        cout << "  SCABS v1.0  |  DSA Project  |  All data structures implemented from scratch";
        _getch();
    }

    // ============================================================
    //  RUN LOOP
    // ============================================================
    void run()
    {
        splash();
        bool go = true;
        while (go)
        {
            int ch = mainMenu();
            switch (ch)
            {
            case 1:
                addClassroom();
                break;
            case 2:
                removeClassroom();
                break;
            case 3:
                viewAll();
                break;
            case 4:
                bookRoom();
                break;
            case 5:
                cancelBooking();
                break;
            case 6:
                markEmpty();
                break;
            case 7:
                undoAction();
                break;
            case 8:
                rescheduleClass();
                break;
            case 9:
                recommendRooms();
                break;
            case 10:
                findFree();
                break;
            case 11:
                roomInfo();
                break;
            case 12:
                viewQueue();
                break;
            case 13:
                categorizeRoom();
                break;
            case 14:
                loadSample();
                okMsg("Sample data loaded — 8 classrooms added to Linked List, BST, and Hash Table.");
                _getch();
                break;
            case 0:
                go = false;
                break;
            default:
                errMsg("Invalid choice! Please enter a number from the menu.");
                _getch();
            }
        }

        cls();
        cursor(true);
        color(BGREEN);
        putC(0, 18, Const::SCR_W, "Thank you for using SCABS!", BGREEN);
        putC(0, 20, Const::SCR_W, "Smart Classroom Availability & Booking System", BCYAN);
        putC(0, 22, Const::SCR_W, "All data structures implemented manually — DSA Project", BWHITE);
        at(0, 26);
        color(WHITE);
    }
}

// ============================================================
//  ENTRY POINT
// ============================================================
int main()
{
    GUI::init();
    Sys::init();
    Scr::run();
    return 0;
}