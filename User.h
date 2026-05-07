#pragma once
#include <iostream>
#include <string>

#include "Interface.h"
#include "CartSystem.h"
#include "ExceptionHandlingCode.h"
#include "Product.h"              
#include "PaymentStrategies.h"    

using namespace std;

class User : public Observer {
protected:
    int id;
    string name;
    string lastNotification;

public:
    User(int id, string name) : id(id), name(name) {}
    virtual ~User() = default;

    virtual void displayMenu() = 0;

    void update(string message) override {
        lastNotification = message;
        cout << "Notification for " << name << ": " << message << endl;
    }

    string getLastNotification() {
        return lastNotification;
    }
};

class Admin : public User {
public:
    Admin(int id, string name) : User(id, name) {}

    void displayMenu() override {
        cout << "Admin Menu" << endl;
    }

    void displayMenu(Inventory* inventory) {
        int choice = 0;

        while (choice != 5) {
            cout << "\n=== ADMIN CONTROL PANEL ===" << endl;
            cout << "1. Add New Product\n2. Update Stock\n3. Remove Product\n4. View Last Notification\n5. Logout\nChoice: ";

            choice = ValidateIntegerInput();

            if (choice == 1) {
                int pId, stock;
                string pName;
                double price;

                cout << "Enter ID: ";
                pId = ValidateIntegerInput();

                cout << "Enter Name: ";
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

                cout << "Enter Quantity Change: ";
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
            else if (choice != 5) {
                cout << "Invalid choice!\n";
            }
        }
    }
};

class Customer : public User {
private:
    Cart cart;

public:
    Customer(int id, string name) : User(id, name), cart(5) {}

    void displayMenu() override {
        cout << "Customer Menu" << endl;
    }

    void displayMenu(Inventory* inventory) {
        int choice = 0;

        while (choice != 5) {
            cout << "\n=== CUSTOMER SHOPPING MENU ===" << endl;
            cout << "1. View Catalog\n2. Add to Cart\n3. View Cart\n4. Checkout\n5. Logout\nChoice: ";

            choice = ValidateIntegerInput();

            if (choice == 1) {
                cout << "\n--- Store Catalog ---\n";
                for (int i = 0; i < inventory->getCount(); i++) {
                    Product* p = inventory->getProductAt(i);
                    cout << "[" << p->getId() << "] " << p->getName()
                        << " - $" << p->getPrice()
                        << " (" << p->getStock() << " left)\n";
                }
            }

            else if (choice == 2) {
                int pId, qty;

                cout << "Enter Product ID: ";
                pId = ValidateIntegerInput();

                cout << "Enter Quantity: ";
                qty = ValidateIntegerInput();

                Product* p = inventory->findProduct(pId);

                if (p != nullptr && p->getStock() >= qty) {
                    cart.addItem(new CartItem(*p, qty));
                    cout << "Added " << qty << "x " << p->getName() << " to cart!\n";
                }
                else {
                    cout << "Error! Insufficient stock or product not found!\n";
                }
            }

            else if (choice == 3) {
                cout << "\n--- Your Cart ---\n";

                if (cart.getItemsCount() == 0) {
                    cout << "Your cart is empty.\n";
                }
                else {
                    for (int i = 0; i < cart.getItemsCount(); i++) {
                        CartComponent* item = cart.getItemAt(i);
                        cout << "- Item " << i + 1 << ": $" << item->getPrice() << "\n";
                    }
                    cout << ">>> Total: $" << cart.getPrice() << " <<<\n";
                }
            }

            else if (choice == 4) {
                if (cart.getItemsCount() == 0) {
                    cout << "Error! Cannot checkout an empty cart!\n";
                    continue;
                }

                double finalTotal = cart.getPrice();
                cout << "\nProcessing Order... Total is $" << finalTotal << endl;

                int itemCount = cart.getItemsCount();

                CartItem** extractedItems = new CartItem * [itemCount];
                for (int i = 0; i < itemCount; i++) {
                    extractedItems[i] = (CartItem*)cart.getItemAt(i);
                }

                int orderID = rand() % 1000 + 1000;

                Order* currentOrder = new Order(orderID, extractedItems, itemCount, finalTotal);

                delete[] extractedItems;

                int payChoice = 0;
                while (payChoice != 1 && payChoice != 2) {
                    cout << "Select Payment:\n1. Cash\n2. Card: ";
                    payChoice = ValidateIntegerInput();
                }

                PaymentMethod* strategy = nullptr;

                if (payChoice == 1)
                    strategy = new CashPayment();
                else
                    strategy = new CardPayment();

                currentOrder->processOrder(strategy);

                for (int i = 0; i < itemCount; i++) {
                    CartItem* item = (CartItem*)cart.getItemAt(i);
                    inventory->updateStock(
                        item->getProduct().getId(),
                        -item->getQuantity()
                    );
                }

                cout << "Stock updated and order completed.\n";

                cart.clearCart();

                delete strategy;
                delete currentOrder;
            }
        }
    }

    Cart& getCart() {
        return cart;
    }
};
