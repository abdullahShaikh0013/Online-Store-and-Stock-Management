#pragma once
#include <iostream>
using namespace std;

int ValidateIntegerInput() {
    int input;
    while (true) {
        try {
            if (!(cin >> input)) {
                throw runtime_error("Invalid Input Type");
            }
            return input;
        }
        catch (const runtime_error& e) {
            cout << "[Exception] " << e.what() << ". Please enter a valid input: ";
            cin.clear(); // Reset the error flags
            cin.ignore(1000, '\n'); // Discard the bad characters
        }
    }
}

string ValidateStringInput() {
    string input;
    while (true) {
        try {
            if (!(cin >> input)) {
                throw runtime_error("Invalid Input Type");
            }
            return input;
        }
        catch (const runtime_error& e) {
            cout << "[Exception] " << e.what() << ". Please enter a valid input: ";
            cin.clear(); // Reset the error flags
            cin.ignore(1000, '\n'); // Discard the bad characters
        }
    }
}

double ValidateDoubleInput() {
    double input;
    while (true) {
        try {
            if (!(cin >> input)) {
                throw runtime_error("Invalid Input Type");
            }
            return input;
        }
        catch (const runtime_error& e) {
            cout << "[Exception] " << e.what() << ". Please enter a valid input: ";
            cin.clear(); // Reset the error flags
            cin.ignore(1000, '\n'); // Discard the bad characters
        }
    }
}