#include<iostream>
using namespace std;

// Base class A
class A {
public:
    int a_value;
    A() {
        a_value = 10;
        cout << "A constructor, a_value = " << a_value << endl;
    }
    ~A() {
        cout << "A DESTRUCTOR" << endl;
    }
    void displayA() {
        cout << "A's value: " << a_value << endl;
    }
};

// Class B inherits from A
class B : public A {
public:
    int b_value;
    B() {
        b_value = 20;
        cout << "B constructor, b_value = " << b_value << endl;
    }
    ~B() {
        cout << "B DESTRUCTOR" << endl;
    }
    void displayB() {
        cout << "B's value: " << b_value << endl;
    }
};

// Class C (independent base class)
class C {
public:
    int c_value;
    C() {
        c_value = 30;
        cout << "C constructor, c_value = " << c_value << endl;
    }
    ~C() {
        cout << "C DESTRUCTOR" << endl;
    }
    void displayC() {
        cout << "C's value: " << c_value << endl;
    }
};

// Class D inherits from both B and C
class D : public B, public C {
public:
    int d_value;
    D() {
        d_value = 40;
        cout << "D constructor, d_value = " << d_value << endl;
    }
    ~D() {
        cout << "D DESTRUCTOR" << endl;
    }
    void displayD() {
        cout << "D's value: " << d_value << endl;
    }

    // Display all values, resolving ambiguity by specifying the base class
    void displayAll() {
        cout << "\nDisplaying all values:" << endl;
        B::displayA();   // From A via B
        B::displayB();   // From B
        C::displayC();   // From C
        displayD();      // From D
    }
};

int main() {
    D obj;

    obj.displayAll();

    return 0;
}

