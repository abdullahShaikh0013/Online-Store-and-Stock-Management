#pragma once
#include <iostream>
#include "Interface.h"
#include <string>
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
    int getStock() const {
        return stock;
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
        notify("New Item: " + p->getName() + " is now available in the catalog.");
    }

    Product* findProduct(int id) {
        for (int i = 0; i < count; i++) {
            if (products[i]->getId() == id) {
                return products[i];
            }
        }
        return nullptr;
    }

    void updateStock(int id, int quantityChange) {
        Product* p = findProduct(id);
        if (p != nullptr) {
            if (quantityChange > 0) {
                p->increaseStock(quantityChange);
                notify("RESTOCK: " + p->getName() + " stock increased by " + to_string(quantityChange));
            }
            else if (quantityChange < 0) {
                p->reduceStock(-quantityChange); // Pass as positive number to reduceStock (See reduceStock() code in Product)

                if (p->getStock() == 0) {
                    notify("CRITICAL: " + p->getName() + " is completely out of stock!");
                }
                else if (p->getStock() <= 5) {
                    notify("LOW STOCK: Only " + to_string(p->getStock()) + " left of " + p->getName());
                }
            }
        }
        else {
            cout << "Error! Cannot update stock. Product ID " << id << " not found." << endl;
        }
    }

    void removeItem(int id) {
        int targetIndex = -1;
        for (int i = 0; i < count; i++) {
            if (products[i]->getId() == id) {
                targetIndex = i;
                break;
            }
        }
        if (targetIndex != -1) {
            string removedName = products[targetIndex]->getName();

            // DELETE the actual Product object from the heap
            delete products[targetIndex];

            // Shift all subsequent pointers ONE space to the LEFT
            for (int i = targetIndex; i < count - 1; i++) {
                products[i] = products[i + 1];
            }
            products[count - 1] = nullptr;
            count--;
            notify("REMOVED: " + removedName + " has been discontinued and removed from the catalog.");
        }
        else {
            cout << "Error! Cannot remove item. Product ID " << id << " not found." << endl;
        }
    }

    void notify(string message) override {
        for (int i = 0; i < observerCount; i++) {
            observers[i]->update(message);
        }
    }
};