#include <iostream>
#include <memory>

using namespace std;

class Ex {};

// base class #1
struct B1 {
	B1() {
		cout << __FUNCTION__ << " called for " << id_ << endl;

	}

	~B1() {
		cout << __FUNCTION__ << " called for " << id_ << endl;
	}

	static int id;
	int id_ = id++;
};

int B1::id = 0;

// base class #2
// May throw at ctor
struct B2 {
	B2() {
		cout << __FUNCTION__ << " called for " << id_ << endl;
		if (throw_ex) {
			throw Ex();
		}
	}

	~B2() {
		cout << __FUNCTION__ << " called for " << id_ << endl;
	}

	static int id;
	static bool throw_ex;
	int id_ = id++;
};

int B2::id = 0;
bool B2::throw_ex = false;

// a class which will be pointed to by a pointer inside a class
// May throw at ctor
struct M {
	M() {
		cout << __FUNCTION__ << " called for " << id_ << endl;
		if (throw_ex) {
			throw Ex();
		}
	}

	~M() {
		cout << __FUNCTION__ << " called for " << id_ << endl;
	}

	static int id;
	static bool throw_ex;
	int id_ = id++;
};

int M::id = 0;
bool M::throw_ex = false;

// a class which is inherited from multiple base classes
struct D : public B1, public B2 {
	D() {
		cout << __FUNCTION__ << " called for (" << B1::id_ << "," << B2::id_ << ")" << endl;
	}

	~D() {
		cout << __FUNCTION__ << " called for (" << B1::id_ << "," << B2::id_ << ")" << endl;
	}
};

// a class which has a member pointer to a class
struct E {
	E() : pD_{new D()}, pM_{new M()} {
		cout << __FUNCTION__ << " called for (" << pD_->B1::id_ << "," << pD_->B2::id_ << ")" << endl;
	}

	~E() {
		cout << __FUNCTION__ << " called for (" << pD_->B1::id_ << "," << pD_->B2::id_ << ")" << endl;
	}

	unique_ptr<D> pD_;
	unique_ptr<M> pM_;
};

struct E1 {
	E1() : pD_{nullptr}, pM_{nullptr}, initialized_{false} {}
	
	void Init() {
		cout << __FUNCTION__ << " called for (" << pD_->B1::id_ << "," << pD_->B2::id_ << ")" << endl;
		try {
			pD_ = new D();
			pM_ = new M();
		}
		catch (...) {
			delete pM_;
			pM_ = nullptr;
			delete pD_;
			pD_ = nullptr;
			rethrow;
		}
		initialized_ = true;
	}

	~E1() {
		cout << __FUNCTION__ << " called for (" << pD_->B1::id_ << "," << pD_->B2::id_ << ")" << endl;
		delete pM_;
		delete pD_;
	}

	D* pD_;
	M* pM_;
	bool initialized_;
};

int main() {
	// Final fix for case #4:
	// This shows what will happen when an exception is thrown during construction
	// M will throw an exception
	// An object which is pointed to by a member pointer will be properly destroyed by unique_ptr
	cout << "Case #4: " << endl;
	M::throw_ex = true;
	E* pE = nullptr;
	try {
		pE = new E();
	}
	catch (Ex&) {
		cout << "caught exception" << endl;
	}
	M::throw_ex = false;
	cout << endl;
}

