#include <iostream>
#include <memory>

using namespace std;

struct A {
	A() {
		cout << __FUNCTION__ << " called for " << id_ << endl;
	}

	~A() {
		cout << __FUNCTION__ << " called for " << id_ << endl;
	}

	static int id;
	static bool throwEx;
	int id_ = id++;
};

int A::id = 0;

struct B : public A {
	B() {
		cout << __FUNCTION__ << " called for " << id_ << endl;
	}

	~B() {
		cout << __FUNCTION__ << " called for " << id_ << endl;
	}
};

int main() {
	A* pA{ new A[3] };
	delete pA;
	// delete[] pA;
}

