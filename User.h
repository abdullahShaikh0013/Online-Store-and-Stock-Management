#pragma once
#include <iostream>
#include "Interface.h"
#include "CartSystem.h"
using namespace std;

class User : public Observer {
protected:
    int id;
    string name;
public:
    User(int id, string name) : id(id), name(name) { }

    virtual ~User() = default;
    virtual void displayMenu() = 0;

    void update(string message) override {
        cout << "Notification for " << name << ": " << message << endl;
    }
};

class Admin : public User {
public:
    Admin(int id, string name) : User(id, name) { }

    void displayMenu() override { 
        cout << "Admin Menu" << endl; 
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
    Cart& getCart() { 
        return cart; 
    }
};