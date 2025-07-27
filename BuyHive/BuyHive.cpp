#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <limits>
using namespace std;

#define MAX_PRODUCTS 10
#define MAX_CART_SIZE 10

// ------------------ Product Classes ------------------

class Product {
protected:
    int id;
    string name;
    double price;

public:
    Product() : id(0), name(""), price(0.0) {}

    Product(int pid, string pname, double pprice)
        : id(pid), name(pname), price(pprice) {}

    virtual void display() {
        cout << "ID: " << id << " | Name: " << name << " | Price: Rs." << price << endl;
    }

    int getId() { return id; }
    string getName() { return name; }
    double getPrice() { return price; }
};

class Electronics : public Product {
public:
    Electronics(int pid, string pname, double pprice) : Product(pid, pname, pprice) {}

    void display() override {
        cout << "[Electronics] ";
        Product::display();
    }
};

class Clothing : public Product {
public:
    Clothing(int pid, string pname, double pprice) : Product(pid, pname, pprice) {}

    void display() override {
        cout << "[Clothing]    ";
        Product::display();
    }
};

// ------------------ Cart Class ------------------

class Cart {
private:
    Product* items[MAX_CART_SIZE];
    int itemCount;

public:
    Cart() : itemCount(0) {}

    void addProduct(Product* p) {
        if (itemCount < MAX_CART_SIZE) {
            items[itemCount++] = p;
            cout << p->getName() << " added to cart.\n";
            cout << "Total Bill: Rs." << getTotalBill() << endl;
        } else {
            cout << "Cart is full!\n";
        }
    }

    void removeProduct(int pid) {
        for (int i = 0; i < itemCount; i++) {
            if (items[i]->getId() == pid) {
                cout << items[i]->getName() << " removed from cart.\n";
                for (int j = i; j < itemCount - 1; j++) {
                    items[j] = items[j + 1];
                }
                itemCount--;
                return;
            }
        }
        cout << "Product not found in cart!\n";
    }

    double getTotalBill() {
        double total = 0;
        for (int i = 0; i < itemCount; i++) {
            total += items[i]->getPrice();
        }
        return total;
    }

    bool isEmpty() {
        return itemCount == 0;
    }

    friend void displayCart(Cart& c);
    friend class User;
};

void displayCart(Cart& c) {
    cout << "\n--- Cart Items ---\n";
    if (c.itemCount == 0) {
        cout << "Cart is empty!\n";
        return;
    }
    for (int i = 0; i < c.itemCount; i++) {
        c.items[i]->display();
    }
    cout << "Total Bill: Rs." << c.getTotalBill() << endl;
}

// ------------------ User Class ------------------

class User {
private:
    string username, password;

public:
    User(string uname, string pass) : username(uname), password(pass) {}

    string getUsername() { return username; }

    bool isDuplicateUsername() {
        ifstream file("users.txt");
        string uname, pass;
        while (file >> uname >> pass) {
            if (uname == username) return true;
        }
        return false;
    }

    void registerUser() {
        if (isDuplicateUsername()) {
            cout << "Username already exists! Try a different one.\n";
            return;
        }
        ofstream file("users.txt", ios::app);
        file << username << " " << password << "\n";
        file.close();
        cout << "Registration successful!\n";
    }

    bool loginUser() {
        ifstream file("users.txt");
        string uname, pass;
        while (file >> uname >> pass) {
            if (uname == username && pass == password) {
                cout << "Login successful!\n";
                return true;
            }
        }
        cout << "Invalid credentials!\n";
        return false;
    }

    void saveOrderToFile(Cart& cart, const string& address) {
        ofstream file("orders.txt", ios::app);
        file << "User: " << username << "\n";
        file << "Shipping Address: " << address << "\n";
        file << "Items:\n";
        for (int i = 0; i < cart.itemCount; i++) {
            file << "- " << cart.items[i]->getName() << " Rs." << cart.items[i]->getPrice() << "\n";
        }
        file << "Total: Rs." << cart.getTotalBill() << "\n";
        file << "-----------------------------------\n";
        file.close();
    }

    void saveAddress(const string& address) {
        ofstream file("addresses.txt", ios::app);
        file << username << ":" << address << "\n";
        file.close();
    }

    vector<string> getSavedAddresses() {
        vector<string> addresses;
        ifstream file("addresses.txt");
        string line;
        while (getline(file, line)) {
            size_t delim = line.find(':');
            if (line.substr(0, delim) == username) {
                addresses.push_back(line.substr(delim + 1));
            }
        }
        return addresses;
    }

    void viewOrderHistory() {
        ifstream file("orders.txt");
        string line;
        bool found = false;
        while (getline(file, line)) {
            if (line.find("User: " + username) != string::npos) {
                found = true;
                cout << "\n" << line << "\n";
                while (getline(file, line) && line != "-----------------------------------") {
                    cout << line << "\n";
                }
                cout << "-----------------------------------\n";
            }
        }
        if (!found) cout << "No order history found for user.\n";
    }
};

// ------------------ Input Validation ------------------

void getValidatedIntInput(const string& prompt, int& input) {
    while (true) {
        cout << prompt;
        cin >> input;
        if (cin.fail()) {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cout << "Invalid input! Please enter an integer.\n";
        } else {
            break;
        }
    }
}

// ------------------ Main ------------------

int main() {
    Product* products[MAX_PRODUCTS] = {
        new Electronics(1, "Laptop", 80000),
        new Electronics(2, "Smartphone", 15000),
        new Electronics(3, "Headphone", 1000),
        new Electronics(4, "Smartwatch", 2000),
        new Clothing(5, "T-Shirt", 200),
        new Clothing(6, "Jeans", 2400),
        new Clothing(7, "Saree", 4500),
        new Clothing(8, "Trousers", 900),
        new Clothing(9, "Shorts", 700),
        new Clothing(10, "Hoodie", 2000)
    };

    Cart cart;
    User* currentUser = nullptr;
    int choice;

    while (true) {
        cout << "\n1. Register\n2. Login\n3. Browse Products\n4. Add to Cart\n5. View Cart\n6. Remove from Cart\n7. Place Order\n8. View Order History\n9. Exit\n";
        getValidatedIntInput("Enter choice: ", choice);

        try {
            if (choice < 1 || choice > 9)
                throw invalid_argument("Invalid menu choice! Please enter between 1 and 9.");

            if (choice == 1) {
                string uname, pass;
                cout << "Enter username: ";
                cin >> uname;
                cout << "Enter password: ";
                cin >> pass;
                User u(uname, pass);
                u.registerUser();
            }

            else if (choice == 2) {
                string uname, pass;
                cout << "Enter username: ";
                cin >> uname;
                cout << "Enter password: ";
                cin >> pass;
                User* temp = new User(uname, pass);
                if (temp->loginUser()) {
                    currentUser = temp;
                } else {
                    delete temp;
                    continue;
                }
            }

            else if (choice == 3) {
                cout << "\n--- Available Products ---\n";
                for (int i = 0; i < MAX_PRODUCTS; i++) {
                    products[i]->display();
                }
            }

            else if (choice == 4) {
                if (!currentUser) {
                    cout << "Please login first to add products to cart.\n";
                    continue;
                }
                int pid;
                getValidatedIntInput("Enter Product ID to add to cart: ", pid);
                bool found = false;
                for (int i = 0; i < MAX_PRODUCTS; i++) {
                    if (products[i]->getId() == pid) {
                        cart.addProduct(products[i]);
                        found = true;
                        break;
                    }
                }
                if (!found) cout << "Invalid Product ID!\n";
            }

            else if (choice == 5) {
                displayCart(cart);
            }

            else if (choice == 6) {
                if (!currentUser) {
                    cout << "Please login first to remove products from cart.\n";
                    continue;
                }
                int pid;
                getValidatedIntInput("Enter Product ID to remove from cart: ", pid);
                cart.removeProduct(pid);
            }

            else if (choice == 7) {
                if (!currentUser) {
                    cout << "Please login to place order.\n";
                    continue;
                }
                displayCart(cart);
                if (cart.isEmpty()) {
                    cout << "Cart is empty. Cannot place order!\n";
                } else {
                    vector<string> savedAddresses = currentUser->getSavedAddresses();
                    string selectedAddress;
                    if (!savedAddresses.empty()) {
                        cout << "\nSaved Addresses:\n";
                        for (int i = 0; i < savedAddresses.size(); i++) {
                            cout << i + 1 << ". " << savedAddresses[i] << endl;
                        }
                        cout << savedAddresses.size() + 1 << ". Enter a new address\n";
                        int addrChoice;
                        getValidatedIntInput("Choose an address: ", addrChoice);
                        cin.ignore();

                        if (addrChoice >= 1 && addrChoice <= savedAddresses.size()) {
                            selectedAddress = savedAddresses[addrChoice - 1];
                        } else if (addrChoice == savedAddresses.size() + 1) {
                            cout << "Enter new address: ";
                            getline(cin, selectedAddress);
                            currentUser->saveAddress(selectedAddress);
                        } else {
                            cout << "Invalid choice!\n";
                            continue;
                        }
                    } else {
                        cin.ignore();
                        cout << "Enter shipping address: ";
                        getline(cin, selectedAddress);
                        currentUser->saveAddress(selectedAddress);
                    }

                    cout << "Order placed successfully!\n";
                    cout << "Shipping to: " << selectedAddress << endl;
                    cout << "Total Amount Paid: Rs." << cart.getTotalBill() << endl;
                    currentUser->saveOrderToFile(cart, selectedAddress);
                    cart = Cart(); // reset cart
                }
            }

            else if (choice == 8) {
                if (!currentUser) {
                    cout << "Please login to view order history.\n";
                    continue;
                }
                currentUser->viewOrderHistory();
            }

            else if (choice == 9) {
                cout << "Exiting program...\n";
                break;
            }

        } catch (const invalid_argument& e) {
            cout << "Exception: " << e.what() << endl;
        }
    }

    for (int i = 0; i < MAX_PRODUCTS; i++) {
        delete products[i];
    }
    delete currentUser;

    return 0;
}
