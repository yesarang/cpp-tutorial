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
	unique_ptr<A[]> pA{ new A[3] };
	unique_ptr<B[]> pB{ new B[3] };
}

