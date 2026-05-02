#pragma once
#include <iostream>
#include "CartSystem.h"
using namespace std;

class PaymentMethod {
public:
    virtual ~PaymentMethod() = default;
    virtual void pay(double amount) = 0;
};

class CashPayment : public PaymentMethod {
public:
    void pay(double amount) override { 
        cout << "Paid $" << amount << " via Cash." << endl;
    }
};

class CardPayment : public PaymentMethod {
public:
    void pay(double amount) override { 
        cout << "Paid $" << amount << " via Card." << endl; 
    }
};

class Order {
private:
    int orderID;
    CartItem** items; // Array of pointers for items
    int itemCount;
    double total;
public:
    Order(int id, CartItem** orderItems, int count, double orderTotal) : orderID(id), items(orderItems), itemCount(count), total(orderTotal) { }

    Order(int id, CartItem** sourceItems, int count, double orderTotal) {
        // Implementing Deep Copy from the Customer's Cart to the final Order

        orderID = id;
        itemCount = count;
        total = orderTotal;

        items = new CartItem * [itemCount];

        for (int i = 0; i < itemCount; i++) {
            Product p = sourceItems[i]->getProduct();
            int q = sourceItems[i]->getQuantity();
            items[i] = new CartItem(p, q);
        }

        cout << "Order #" << orderID << " created successfully with a deep copy." << endl;
    }

    ~Order() {
        // Cleaning up the array of items assigned to this order
        for (int i = 0; i < itemCount; i++) {
            delete items[i];
            items[i] = nullptr;
        }
        delete[] items;
        items = nullptr;
    }

    void processOrder(PaymentMethod* paymentMethod) {
        cout << "Order ID: " << orderID << endl;
        if (paymentMethod) {
            paymentMethod->pay(total);
        }
    }
};