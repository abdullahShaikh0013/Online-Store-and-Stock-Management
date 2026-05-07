#pragma once
#include <iostream>
using namespace std;

class Observer {
public:
    virtual ~Observer() = default;
    virtual void update(string message) = 0;
};

class Subject {
protected:
    Observer** observers;
    int observerCount;
    int observerCapacity;

    void resizeObservers() {
        observerCapacity *= 2;
        Observer** newObservers = new Observer * [observerCapacity];
        for (int i = 0; i < observerCount; i++) {
            newObservers[i] = observers[i];
        }
        delete[] observers;
        observers = newObservers;
    }

public:
    Subject() : observerCount(0), observerCapacity(5) {
        observers = new Observer * [observerCapacity];
    }
    virtual ~Subject() {
        delete[] observers;
    }
    virtual void attach(Observer* o) {
        if (observerCount >= observerCapacity) resizeObservers();
        observers[observerCount++] = o;
    }
    virtual void notify(string message) = 0;
}; 
