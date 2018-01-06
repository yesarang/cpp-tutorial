#include <iostream>
#include <memory>
#include <exception>

using namespace std;

class Ex {};
class Ex2 {};

// base class #1
struct B1 {
	B1() {
		cout << __FUNCTION__ << " called for " << id_ << endl;

	}

	~B1() {
		cout << __FUNCTION__ << " called for " << id_ << endl;
		cout.flush();
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

	~B2() noexcept(false) {
		cout << __FUNCTION__ << " called for " << id_ << endl;
		cout.flush();
		if (throw_ex_at_dtor) {
			if (uncaught_exception()) {
				cout << __FUNCTION__ << " is in exception context" << endl;
				cout.flush();
			}
			throw Ex2();
		}
	} 

	static int id;
	static bool throw_ex;
	static bool throw_ex_at_dtor;
	int id_ = id++;
};

int B2::id = 0;
bool B2::throw_ex = false;
bool B2::throw_ex_at_dtor = false;

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
		cout.flush();
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

	~D() noexcept(false) {
		cout << __FUNCTION__ << " called for (" << B1::id_ << "," << B2::id_ << ")" << endl;
		cout.flush();
	}
};

// a class which has a member pointer to a class
struct E {
	E() : pD_{new D()}, pM_{new M()} {
		cout << __FUNCTION__ << " called for (" << pD_->B1::id_ << "," << pD_->B2::id_ << ")" << endl;
		cout.flush();
	}

	~E() noexcept(false) {
		cout << __FUNCTION__ << " called for (" << pD_->B1::id_ << "," << pD_->B2::id_ << ")" << endl;
		cout.flush();
	}

	int getB1Id() const {
		return pD_->B1::id_;
	}

	unique_ptr<D> pD_;
	unique_ptr<M> pM_;
};

void new_mem_ptr_good_fix() noexcept(false) {
	// Final fix for case #4 can't work with dtor exception because
	// unique_ptr's dtor has noexcept(true) property, in which case
	// noexcept specification is violated and in result, 
	// terminate() will be called. Beware that pD_ is a unique_ptr and
	// delete pD_ will be called in the process of destruction of E.
	// C++ standard library assume that no dtor throws an exception.
	cout << "Final fix for case #4: " << endl;
	unique_ptr<E> pE{ make_unique<E>() };
	cout << "id of pE = " << pE->getB1Id() << endl;
}

void dtor_except_ex() {
	B2 b2;
}

int main() {
	B2::throw_ex_at_dtor = true;

	try {
		dtor_except_ex();
	}
	catch (...) {
		cout << "caught an exception" << endl;
	}

	M::throw_ex = true;
	B2::throw_ex_at_dtor = true;

	try {
		new_mem_ptr_good_fix();
	}
	catch (...) {
	  	cout << "caught an exception" << endl;
	}
}

