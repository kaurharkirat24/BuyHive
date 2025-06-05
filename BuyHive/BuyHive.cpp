#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <map>
#include <limits>
#include <sstream>
using namespace std;

#define MAX_PRODUCTS 100
#define MAX_CART_SIZE 100

// --- Product Classes ---

class Product {
protected:
    int id;
    string name;
    double price;
public:
    Product() : id(0), name(""), price(0.0) {}
    Product(int pid, string pname, double pprice) : id(pid), name(pname), price(pprice) {}
    virtual void display() {
        cout << "ID: " << id << " | Name: " << name << " | Price: Rs." << price << endl;
    }
    int getId() { return id; }
    string getName() { return name; }
    double getPrice() { return price; }
    void setName(const string& n) { name = n; }
    void setPrice(double p) { price = p; }
    virtual int getType() const { return 0; } // 1=Electronics, 2=Clothing
};

class Electronics : public Product {
public:
    Electronics(int pid, string pname, double pprice) : Product(pid, pname, pprice) {}
    void display() override {
        cout << "[Electronics] ";
        Product::display();
    }
    int getType() const override { return 1; }
};

class Clothing : public Product {
public:
    Clothing(int pid, string pname, double pprice) : Product(pid, pname, pprice) {}
    void display() override {
        cout << "[Clothing]    ";
        Product::display();
    }
    int getType() const override { return 2; }
};

// --- Product File I/O ---

vector<Product*> loadProducts(const string& filename) {
    vector<Product*> products;
    ifstream file(filename);
    string line;
    while (getline(file, line)) {
        stringstream ss(line);
        int id, type;
        string name;
        double price;
        ss >> id >> type;
        ss.ignore();
        getline(ss, name, '|');
        ss >> price;
        if (type == 1)
            products.push_back(new Electronics(id, name, price));
        else if (type == 2)
            products.push_back(new Clothing(id, name, price));
    }
    return products;
}

void saveProducts(const string& filename, const vector<Product*>& products) {
    ofstream file(filename);
    for (auto* p : products) {
        file << p->getId() << " " << p->getType() << " " << p->getName() << "|" << p->getPrice() << endl;
    }
}

// --- Cart with Quantities ---

class Cart {
private:
    map<int, int> items; // productId -> quantity
public:
    void addProduct(int pid, int qty) {
        items[pid] += qty;
        cout << "Added quantity " << qty << " of product ID " << pid << " to cart.\n";
    }
    void removeProduct(int pid) {
        if (items.count(pid)) {
            items.erase(pid);
            cout << "Product ID " << pid << " removed from cart.\n";
        } else {
            cout << "Product not found in cart!\n";
        }
    }
    double getTotalBill(const vector<Product*>& products) {
        double total = 0;
        for (auto& [pid, qty] : items) {
            for (auto* p : products) {
                if (p->getId() == pid) total += p->getPrice() * qty;
            }
        }
        return total;
    }
    void display(const vector<Product*>& products) {
        cout << "\n--- Cart Items ---\n";
        if (items.empty()) {
            cout << "Cart is empty!\n";
            return;
        }
        for (auto& [pid, qty] : items) {
            for (auto* p : products) {
                if (p->getId() == pid) {
                    p->display();
                    cout << "Quantity: " << qty << endl;
                }
            }
        }
        cout << "Total Bill: Rs." << getTotalBill(products) << endl;
    }
    bool isEmpty() { return items.empty(); }
    map<int, int>& getItems() { return items; }
    void clear() { items.clear(); }
};

// --- User and Admin ---

class User {
protected:
    string username, password;
public:
    User(string uname, string pass) : username(uname), password(pass) {}
    virtual bool isAdmin() { return false; }
    string getUsername() { return username; }
    void registerUser() {
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
        file.close();
        cout << "Invalid credentials!\n";
        return false;
    }
};

class Admin : public User {
public:
    Admin(string uname, string pass) : User(uname, pass) {}
    bool isAdmin() override { return true; }
};

// --- Order History ---

void saveOrder(const string& username, Cart& cart, const vector<Product*>& products, const string& address) {
    ofstream file("orders.txt", ios::app);
    file << username << "|";
    for (auto& [pid, qty] : cart.getItems()) {
        file << pid << ":" << qty << ",";
    }
    file << "|" << cart.getTotalBill(products) << "|" << address << endl;
}

void viewOrderHistory(const string& username, const vector<Product*>& products) {
    ifstream file("orders.txt");
    string line;
    cout << "\n--- Order History for " << username << " ---\n";
    bool found = false;
    while (getline(file, line)) {
        stringstream ss(line);
        string user, items, total, address;
        getline(ss, user, '|');
        getline(ss, items, '|');
        getline(ss, total, '|');
        getline(ss, address);
        if (user == username) {
            found = true;
            cout << "Items: ";
            stringstream itemss(items);
            string item;
            while (getline(itemss, item, ',')) {
                if (item.empty()) continue;
                int pid, qty;
                sscanf(item.c_str(), "%d:%d", &pid, &qty);
                for (auto* p : products) {
                    if (p->getId() == pid) {
                        cout << p->getName() << " x" << qty << "; ";
                    }
                }
            }
            cout << "\nTotal: Rs." << total << "\nAddress: " << address << "\n---\n";
        }
    }
    if (!found) cout << "No orders found.\n";
}

// --- Input Validation ---

int getIntInput(const string& prompt, int min, int max) {
    int val;
    while (true) {
        cout << prompt;
        cin >> val;
        if (cin.fail() || val < min || val > max) {
            cout << "Invalid input. Try again.\n";
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
        } else {
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            return val;
        }
    }
}

double getDoubleInput(const string& prompt, double min) {
    double val;
    while (true) {
        cout << prompt;
        cin >> val;
        if (cin.fail() || val < min) {
            cout << "Invalid input. Try again.\n";
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
        } else {
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            return val;
        }
    }
}

// --- Main ---

int main() {
    vector<Product*> products = loadProducts("products.txt");
    Cart cart;
    User* currentUser = nullptr;
    bool loggedIn = false, isAdmin = false;

    while (true) {
        cout << "\n--- BuyHive Main Menu ---\n";
        cout << "1. Register\n2. Login\n3. Browse Products\n4. Add to Cart\n5. View Cart\n6. Remove from Cart\n7. Place Order\n8. View Order History\n9. Exit\n";
        if (isAdmin) cout << "10. [Admin] Add Product\n11. [Admin] Edit Product\n12. [Admin] Remove Product\n";
        int choice = getIntInput("Enter choice: ", 1, isAdmin ? 12 : 9);

        if (choice == 1) {
            string uname, pass;
            cout << "Enter username: "; getline(cin, uname);
            cout << "Enter password: "; getline(cin, pass);
            User u(uname, pass);
            u.registerUser();
        } else if (choice == 2) {
            string uname, pass;
            cout << "Enter username: "; getline(cin, uname);
            cout << "Enter password: "; getline(cin, pass);
            if (uname == "admin" && pass == "admin123") {
                currentUser = new Admin(uname, pass);
                loggedIn = true; isAdmin = true;
                cout << "Admin login successful.\n";
            } else {
                User* u = new User(uname, pass);
                if (u->loginUser()) {
                    currentUser = u;
                    loggedIn = true; isAdmin = false;
                } else {
                    delete u;
                }
            }
        } else if (choice == 3) {
            cout << "\n--- Available Products ---\n";
            for (auto* p : products) p->display();
        } else if (choice == 4) {
            if (!loggedIn) { cout << "Please login first.\n"; continue; }
            int pid = getIntInput("Enter Product ID to add to cart: ", 1, 10000);
            int qty = getIntInput("Enter quantity: ", 1, 100);
            bool found = false;
            for (auto* p : products) {
                if (p->getId() == pid) { found = true; break; }
            }
            if (!found) cout << "Invalid Product ID!\n";
            else cart.addProduct(pid, qty);
        } else if (choice == 5) {
            cart.display(products);
        } else if (choice == 6) {
            int pid = getIntInput("Enter Product ID to remove from cart: ", 1, 10000);
            cart.removeProduct(pid);
        } else if (choice == 7) {
            if (!loggedIn) { cout << "Please login first.\n"; continue; }
            cart.display(products);
            if (cart.isEmpty()) {
                cout << "Cart is empty. Cannot place an order!\n";
            } else {
                string address;
                cout << "Enter your shipping address: ";
                getline(cin, address);
                saveOrder(currentUser->getUsername(), cart, products, address);
                cout << "Order placed successfully!\n";
                cart.clear();
            }
        } else if (choice == 8) {
            if (!loggedIn) { cout << "Please login first.\n"; continue; }
            viewOrderHistory(currentUser->getUsername(), products);
        } else if (choice == 9) {
            cout << "Exiting program...\n";
            break;
        } else if (isAdmin && choice == 10) {
            int id = getIntInput("Enter new product ID: ", 1, 10000);
            cout << "Enter name: "; string name; getline(cin, name);
            double price = getDoubleInput("Enter price: Rs.", 1);
            int type = getIntInput("Enter type (1=Electronics, 2=Clothing): ", 1, 2);
            Product* p = (type == 1) ? (Product*)new Electronics(id, name, price) : (Product*)new Clothing(id, name, price);
            products.push_back(p);
            saveProducts("products.txt", products);
            cout << "Product added.\n";
        } else if (isAdmin && choice == 11) {
            int id = getIntInput("Enter product ID to edit: ", 1, 10000);
            for (auto* p : products) {
                if (p->getId() == id) {
                    cout << "Editing "; p->display();
                    cout << "Enter new name: "; string name; getline(cin, name);
                    double price = getDoubleInput("Enter new price: Rs.", 1);
                    p->setName(name); p->setPrice(price);
                    saveProducts("products.txt", products);
                    cout << "Product updated.\n";
                    break;
                }
            }
        } else if (isAdmin && choice == 12) {
            int id = getIntInput("Enter product ID to remove: ", 1, 10000);
            for (auto it = products.begin(); it != products.end(); ++it) {
                if ((*it)->getId() == id) {
                    delete *it;
                    products.erase(it);
                    saveProducts("products.txt", products);
                    cout << "Product removed.\n";
                    break;
                }
            }
        }
    }
    for (auto* p : products) delete p;
    delete currentUser;
    return 0;
}