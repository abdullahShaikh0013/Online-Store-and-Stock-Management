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