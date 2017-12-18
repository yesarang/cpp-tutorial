#include <iostream>

using namespace std;

class Ex {};

struct A {
	A() {
		cout << __FUNCTION__ << " called for " << id_ << endl;
	}

	~A() {
		cout << __FUNCTION__ << " called for " << id_ << endl;
	}

	static int id;
	int id_ = id++;
};

int A::id = 0;

struct B {
	B() {
		cout << __FUNCTION__ << " called for " << id_ << endl;
		if (throwEx && id == 3) {
			throw Ex();
		}
	}

	~B() {
		cout << __FUNCTION__ << " called for " << id_ << endl;
	}

	static int id;
	static bool throwEx;
	int id_ = id++;
};

int B::id = 0;
bool B::throwEx = false;

int main() {
	B::throwEx = true;
	A* pA = nullptr;
	B* pB = nullptr;
	try {
		pA = new A[3];
		pB = new B[3];
	}
	catch (Ex&) {
		cout << "caught exception" << endl;
	}
	delete[] pA;
	delete[] pB;
}

