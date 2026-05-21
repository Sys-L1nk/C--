#include <iostream>
using namespace std;

// 1. Structure for our fiber-optic cable
struct Connection
{
    int city1;
    int city2;
    int cost;
};

// ==========================================
// CUSTOM QUICKSORT IMPLEMENTATION
// ==========================================

// Helper function to swap two connections
void swapConnections(Connection &a, Connection &b)
{
    Connection temp = a;
    a = b;
    b = temp;
}

// Partition function for QuickSort
int partition(Connection arr[], int low, int high)
{
    int pivotCost = arr[high].cost;
    int i = (low - 1); // Index of smaller element

    for (int j = low; j <= high - 1; j++)
    {
        // If current element is smaller than the pivot
        if (arr[j].cost < pivotCost)
        {
            i++;
            swapConnections(arr[i], arr[j]);
        }
    }
    swapConnections(arr[i + 1], arr[high]);
    return (i + 1);
}

// The main QuickSort function
void quickSort(Connection arr[], int low, int high)
{
    if (low < high)
    {
        int pi = partition(arr, low, high);
        quickSort(arr, low, pi - 1);
        quickSort(arr, pi + 1, high);
    }
}

// ==========================================
// DISJOINT-SET (UNION-FIND) WITH RAW ARRAYS
// ==========================================

class DisjointSet
{
private:
    int *parent;
    int *rank;
    int size;

public:
    DisjointSet(int n)
    {
        size = n;
        parent = new int[n];
        rank = new int[n];
        for (int i = 0; i < n; i++)
        {
            parent[i] = i;
            rank[i] = 0;
        }
    }

    // Destructor to prevent memory leaks!
    ~DisjointSet()
    {
        delete[] parent;
        delete[] rank;
    }

    int find(int i)
    {
        if (parent[i] == i)
        {
            return i;
        }
        // Path compression
        return parent[i] = find(parent[i]);
    }

    void unionSets(int i, int j)
    {
        int root_i = find(i);
        int root_j = find(j);

        if (root_i != root_j)
        {
            // Union by Rank
            if (rank[root_i] < rank[root_j])
            {
                parent[root_i] = root_j;
            }
            else if (rank[root_i] > rank[root_j])
            {
                parent[root_j] = root_i;
            }
            else
            {
                parent[root_j] = root_i;
                rank[root_i]++;
            }
        }
    }
};

// ==========================================
// TELECOM NETWORK
// ==========================================

class TelecomNetwork
{
private:
    int numCities;
    Connection *possibleConnections;
    int maxConnections; // Capacity of our raw array
    int numConnections; // Current number of connections added

public:
    TelecomNetwork(int vertices, int maxEdges)
    {
        numCities = vertices;
        maxConnections = maxEdges;
        numConnections = 0;
        possibleConnections = new Connection[maxConnections];
    }

    // Destructor
    ~TelecomNetwork()
    {
        delete[] possibleConnections;
    }

    void addConnection(int u, int v, int cost)
    {
        if (numConnections < maxConnections)
        {
            possibleConnections[numConnections].city1 = u;
            possibleConnections[numConnections].city2 = v;
            possibleConnections[numConnections].cost = cost;
            numConnections++;
        }
        else
        {
            cout << "Array full! Cannot add more connections." << endl;
        }
    }

    void buildOptimalNetwork()
    {
        // 1. Sort the raw array using our custom QuickSort
        quickSort(possibleConnections, 0, numConnections - 1);

        DisjointSet ds(numCities);

        // Raw array to hold the final chosen connections (Max size is V - 1)
        Connection *mst = new Connection[numCities - 1];
        int mstSize = 0;
        int totalMinimumCost = 0;

        // 2. Iterate through the sorted array
        for (int i = 0; i < numConnections; i++)
        {
            Connection currentEdge = possibleConnections[i];

            int root1 = ds.find(currentEdge.city1);
            int root2 = ds.find(currentEdge.city2);

            // If they belong to different sets, add to MST
            if (root1 != root2)
            {
                mst[mstSize] = currentEdge;
                mstSize++;
                totalMinimumCost += currentEdge.cost;
                ds.unionSets(root1, root2);
            }

            // Stop early if we hit exactly V - 1 edges
            if (mstSize == numCities - 1)
            {
                break;
            }
        }

        // --- DISPLAY RESULTS ---
        cout << "---  OPTIMAL TELECOM NETWORK ---" << endl;
        cout << "Minimum Total Installation Cost: $" << totalMinimumCost << "\n"
             << endl;
        cout << "Cables to Install:" << endl;
        for (int i = 0; i < mstSize; i++)
        {
            cout << "City " << mst[i].city1 << " <---> City " << mst[i].city2
                 << "  (Cost: $" << mst[i].cost << ")" << endl;
        }

        // Clean up the temporary array
        delete[] mst;
    }
};

int main()
{
    int totalCities = 5;
    int maxEdges = 10; // We have to define a maximum size for the raw array

    TelecomNetwork network(totalCities, maxEdges);

    network.addConnection(0, 1, 10);
    network.addConnection(0, 2, 6);
    network.addConnection(0, 3, 5);
    network.addConnection(1, 3, 15);
    network.addConnection(2, 3, 4);
    network.addConnection(1, 4, 8);
    network.addConnection(3, 4, 12);

    network.buildOptimalNetwork();

    return 0;
}