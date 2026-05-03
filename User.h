#pragma once
#include <iostream>
#include "Interface.h"
#include "CartSystem.h"
#include "ExceptionHandlingCode.h"
using namespace std;

class User : public Observer {
protected:
    int id;
    string name;
    string lastNotification; // To store message for frontend
public:
    User(int id, string name) : id(id), name(name) { }

    virtual ~User() = default;
    virtual void displayMenu() = 0;

    void update(string message) override {
        lastNotification = message;
        cout << "Notification for " << name << ": " << message << endl;
    }
    string getLastNotification() { // Backend hook for frontend to display the last message
        return lastNotification;
    }
};

class Admin : public User {
public:
    Admin(int id, string name) : User(id, name) { }

    void displayMenu() override { 
        cout << "Admin Menu" << endl; 
    }
    void displayMenu(Inventory* inventory) {
        int choice = 0;
        while (choice != 5) {
            cout << "\n=== ADMIN CONTROL PANEL ===" << endl;
            cout << "1. Add New Product\n2. Update Stock\n3. Remove Product\n4. View Last Notification\n5. Logout\nChoice: ";
            while (choice != 1 && choice != 2 && choice != 3 && choice != 4 && choice != 5) {
                choice = ValidateIntegerInput();
                if (choice != 1 && choice != 2 && choice != 3 && choice != 4 && choice != 5) {
                    cout << "Error! Invalid choice." << endl;
                }
            }

            if (choice == 1) {
                int pId, stock; 
                string pName; 
                double price;
                cout << "Enter ID: "; 
                pId = ValidateIntegerInput();
                cout << "Enter Name (No spaces): ";
                pName = ValidateStringInput();
                cout << "Enter Price: "; 
                price = ValidateDoubleInput(); 
                cout << "Enter Initial Stock: "; 
                stock = ValidateIntegerInput();
                inventory->addProduct(new Product(pId, pName, price, stock));
            }
            else if (choice == 2) {
                int pId, qty;
                cout << "Enter Product ID: "; 
                pId = ValidateIntegerInput();
                cout << "Enter Quantity Change (+ for restock, - for reduction): "; 
                qty = ValidateIntegerInput();
                inventory->updateStock(pId, qty);
            }
            else if (choice == 3) {
                int pId;
                cout << "Enter Product ID to remove: "; 
                pId = ValidateIntegerInput();
                inventory->removeItem(pId);
            }
            else if (choice == 4) {
                cout << "\nLast Notification: " << getLastNotification() << endl;
            }
        }
    }
};

class Customer : public User {
private:
    Cart cart;
public:
    Customer(int id, string name) : User(id, name), cart(5) { }

    void displayMenu() override { 
        cout << "Customer Menu" << endl; 
    }
    void displayMenu(Inventory* inventory) {
        int choice = 0;
        while (choice != 5) {
            cout << "\n=== CUSTOMER SHOPPING MENU ===" << endl;
            cout << "1. View Catalog\n2. Add to Cart\n3. View Cart Total\n4. Checkout\n5. Logout\nChoice: ";
            while (choice != 1 && choice != 2 && choice != 3 && choice != 4 && choice != 5) {
                choice = ValidateIntegerInput();
                if (choice != 1 && choice != 2 && choice != 3 && choice != 4 && choice != 5) {
                    cout << "Error! Invalid choice." << endl;
                }
            }

            if (choice == 1) {
                cout << "\n--- Store Catalog ---" << std::endl;
                for (int i = 0; i < inventory->getProductCount(); i++) {
                    Product* p = inventory->getProductAt(i);
                    cout << "[" << p->getId() << "] " << p->getName() << " - $" << p->getPrice() << " (" << p->getStock() << " left)" << endl;
                }
            }
            else if (choice == 2) {
                int pId, qty;
                std::cout << "Enter Product ID: ";
                pId = ValidateIntegerInput();
                std::cout << "Enter Quantity: "; 
                qty = ValidateIntegerInput();

                Product* p = inventory->findProduct(pId);

                // Validation to ensure product exists and has as much stock as quantity required
                if (p != nullptr && p->getStock() >= qty) {
                    cart.addItem(new CartItem(*p, qty));
                    cout << "Added " << qty << "x " << p->getName() << " to cart!" << endl;
                }
                else {
                    std::cout << "Error! Insufficient stock or product not found!" << endl;
                }
            }
            else if (choice == 3) {
                std::cout << "\n--- Your Cart ---" << endl;
                if (cart.getItemsCount() == 0) {
                    std::cout << "Your cart is empty.\n";
                }
                else {
                    for (int i = 0; i < cart.getItemsCount(); i++) {
                        CartComponent* item = cart.getItemAt(i);
                        cout << "- Item " << i + 1 << ": $" << item->getPrice() << "\n";
                    }
                    std::cout << ">>> Total: $" << cart.getPrice() << " <<<\n";
                }
            }
            else if (choice == 4) {
                if (cart.getItemsCount() == 0) {
                    cout << "Error! Cannot checkout an empty cart!" << endl;
                    continue;
                }
                double finalTotal = cart.getPrice();
                cout << "\nProcessing Order... Total is $" << finalTotal << endl;

                // Extract Items for Order Constructor
                int itemCount = cart.getItemsCount();
                CartItem** extractedItems = new CartItem * [itemCount];
                for (int i = 0; i < itemCount; i++) { 
                    extractedItems[i] = (CartItem*)cart.getItemAt(i);
                }
                // Deep copy into Order
                int generatedOrderID = rand() % 1000 + 1000;
                Order* currentOrder = new Order(generatedOrderID, extractedItems, itemCount, finalTotal);
                delete[] extractedItems;
                extractedItems = nullptr;

                // Select Payment (Strategy Pattern)
                int payChoice = 0;
                while (payChoice != 1 && payChoice != 2) {
                    cout << "Select Payment:\n1. Cash\n2. Card: ";
                    payChoice = ValidateIntegerInput();
                    if (payChoice != 1 && payChoice != 2) {
                        cout << "Error! Invalid choice." << endl;
                    }
                }

                PaymentMethod* strategy = nullptr;
                if (payChoice == 1) {
                    strategy = new CashPayment();
                }
                else {
                    strategy = new CardPayment();
                }
                currentOrder->processOrder(strategy);

                for (int i = 0; i < itemCount; i++) {
                    CartItem* item = (CartItem*)cart.getItemAt(i);
                    int pId = item->getProduct().getId();
                    int qty = item->getQuantity();
                    inventory->updateStock(pId, -qty);
                }

                cout << "Stock successfully updated and Order finalized." << endl;

                cart.clearCart();
                delete strategy;
                strategy = nullptr;
                delete currentOrder;
                currentOrder = nullptr;
            }
        }
    }

    Cart& getCart() { 
        return cart; 
    }
};