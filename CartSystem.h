#pragma once
#include <iostream>
#include "Product.h"
using namespace std;

class CartComponent {
public:
    virtual ~CartComponent() = default;
    virtual double getPrice() = 0;
};

class CartItem : public CartComponent {
private:
    Product product; // Composition as per UML
    int quantity;
public:
    CartItem(Product p, int q) : product(p), quantity(q) { }
    double getPrice() override { 
        return product.getPrice() * quantity; 
    }
    Product getProduct() const {
        return product;
    }
    int getQuantity() const {
        return quantity;
    }
};

class Cart : public CartComponent {
private:
    CartComponent** items;
    int count;
    int capacity;
    void resize() {
        capacity *= 2;
        CartComponent** newItems = new CartComponent * [capacity];
        for (int i = 0; i < count; i++) {
            newItems[i] = items[i];
        }
        delete[] items;
        items = newItems;
    }
public:
    Cart(int cap = 5) : count(0), capacity(cap) {
        items = new CartComponent * [capacity];
    }
    ~Cart() {
        for (int i = 0; i < count; i++) {
            delete items[i];
            items[i] = nullptr;
        }
        delete[] items;
        items = nullptr;
    }

    void addItem(CartComponent* item) {
        if (count >= capacity) {
            resize();
        }
        items[count++] = item;
    }

    void removeItem(int index) {
        if (index >= 0 && index < count) {
            delete items[index];
            for (int i = index; i < count - 1; i++) {
                items[i] = items[i + 1];
            }
            count--;
        }
    }

    // Clear up for next order
    void clearCart() {
        for (int i = 0; i < count; i++) {
            delete items[i];
        }
        count = 0;
        cout << "Cart has been emptied." << endl;
    }

    double getPrice() override {
        double total = 0;
        for (int i = 0; i < count; i++) {
            total += items[i]->getPrice();
        }
        return total;
    }
    int getItemsCount() {
        return count;
    }
    CartComponent* getItemAt(int index) {
        if (index >= 0 && index < count) {
            return items[index];
        }
        return nullptr;
    }
};