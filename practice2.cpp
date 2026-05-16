#include<iostream>
#include<iomanip>
using namespace std;

struct menutypeitems {
    string menu;
    float price;
};

// Load menu items
void getdata(menutypeitems menulist[], int size) {
    menulist[0] = {"Plain Egg", 1.45};
    menulist[1] = {"Bacon and Egg", 2.45};
    menulist[2] = {"Muffin", 0.99};
    menulist[3] = {"French Toast", 1.99};
    menulist[4] = {"Fruit Basket", 2.49};
    menulist[5] = {"Coffee", 0.50};
    menulist[6] = {"Tea", 0.75};
}

// Show menu and take orders
void showdata(menutypeitems menulist[], int size) {
    int choice, quantity;
    float totalbill = 0;

    cout << "\n----- Breakfast Menu -----\n";
    cout << setw(5) << "No" << setw(25) << "Items" << "Price($)" << endl;
    cout << "------------------------------------------" << endl;

    for (int i = 0; i < size; i++) {
        cout << setw(5) << i+1 << setw(25) << menulist[i].menu << menulist[i].price << endl;
    }

    // Order taking loop
    do {
        cout << "\nEnter item number for your choice (0 to finish): ";
        cin >> choice;

        if (choice > 0 && choice <= size) {
            cout << "Enter the quantity for " << menulist[choice-1].menu << ": ";
            cin >> quantity;
            totalbill += menulist[choice-1].price * quantity;
        }
        else if (choice != 0) {
            cout << "Invalid input! Please try again." << endl;
        }

    } while (choice != 0);

    cout << fixed << setprecision(2); // To format to 2 decimal places
    cout << "\nTotal bill: $" << totalbill << endl;
    cout << "Thank you for your order!" << endl;
}

int main() {
    const int menuSize = 7;
    menutypeitems menulist[menuSize];

    getdata(menulist, menuSize);
    showdata(menulist, menuSize);

    return 0;
}
