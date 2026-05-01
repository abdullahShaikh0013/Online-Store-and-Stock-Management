#pragma once
#include <iostream>
#include "Interface.h"
using namespace std;

class Product {
private:
    int id;
    string name;
    double price;
    int stock;
public:
    Product(int id, string name, double price, int stock) : id(id), name(name), price(price), stock(stock) { }

    double getPrice() const {
        return price;
    }
    int getId() const {
        return id;
    }
    string getName() const {
        return name;
    }
    void increaseStock(int quantity) { 
        stock += quantity; 
    }
    void reduceStock(int quantity) { 
        stock -= quantity; 
    }
};

class Inventory : public Subject {
private:
    Product** products;
    int count;
    int capacity;
    void resize() {
        capacity *= 2;
        Product** newProducts = new Product * [capacity];
        for (int i = 0; i < count; i++) {
            newProducts[i] = products[i];
        }
        delete[] products;
        products = newProducts;
    }
public:
    Inventory(int cap = 10) : count(0), capacity(cap) {
        products = new Product * [capacity];
    }
    ~Inventory() {
        for (int i = 0; i < count; i++) {
            delete products[i];
            products[i] = nullptr;
        }
        delete[] products;
        products = nullptr;
    }

    void addProduct(Product* p) {
        if (count >= capacity) {
            resize();
        }
        products[count++] = p;
    }

    void updateStock(int id, int quantity) {
        for (int i = 0; i < count; i++) {
            if (products[i]->getId() == id) {
                products[i]->increaseStock(quantity);
                notify("Update: " + products[i]->getName() + " stock changed.");
                return;
            }
        }
    }

    void notify(string message) override {
        for (int i = 0; i < observerCount; i++) {
            observers[i]->update(message);
        }
    }
};