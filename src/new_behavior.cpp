#include <iostream>
#include <memory>

using namespace std;

class Ex {};

// base class #1
struct B1 {
  B1() { cout << __FUNCTION__ << " called for " << id_ << endl; }

  ~B1() { cout << __FUNCTION__ << " called for " << id_ << endl; }

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

  ~B2() { cout << __FUNCTION__ << " called for " << id_ << endl; }

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

  ~M() { cout << __FUNCTION__ << " called for " << id_ << endl; }

  static int id;
  static bool throw_ex;
  int id_ = id++;
};

int M::id = 0;
bool M::throw_ex = false;

// a class which is inherited from multiple base classes
struct D : public B1, public B2 {
  D() {
    cout << __FUNCTION__ << " called for (" << B1::id_ << "," << B2::id_ << ")"
         << endl;
  }

  ~D() {
    cout << __FUNCTION__ << " called for (" << B1::id_ << "," << B2::id_ << ")"
         << endl;
  }
};

// a class which has a member pointer to a class
struct E {
  E() : pD_{new D()}, pM_{new M()} {
    cout << __FUNCTION__ << " called for (" << pD_->B1::id_ << ","
         << pD_->B2::id_ << ")" << endl;
  }

  ~E() {
    cout << __FUNCTION__ << " called for (" << pD_->B1::id_ << ","
         << pD_->B2::id_ << ")" << endl;
    delete pM_;
    delete pD_;
  }

  D *pD_;
  M *pM_;
};

int main() {
  // Case #1:
  // This shows construction & destruction of an object which is inherited from
  // multiple base classes
  cout << "Case #1: " << endl;
  D *pD{new D()};
  delete pD;
  pD = nullptr;
  cout << endl;

  // Case #2:
  // This shows what will happen when an exception is thrown during construction
  // B2 will throw an exception
  // Partially constructed object will be destroyed properly
  cout << "Case #2: " << endl;
  B2::throw_ex = true;
  try {
    pD = new D();
  } catch (Ex &) {
    cout << "caught exception" << endl;
  }
  B2::throw_ex = false;
  cout << endl;

  // Case #3:
  // This shows construction & destruction of an object whose member pointer
  // points to an object
  cout << "Case #3: " << endl;
  E *pE{new E()};
  delete pE;
  cout << endl;

  // Case #4:
  // This shows what will happen when an exception is thrown during construction
  // M will throw an exception
  // An object which is pointed to by a member pointer will leak
  cout << "Case #4: " << endl;
  M::throw_ex = true;
  try {
    pE = new E();
  } catch (Ex &) {
    cout << "caught exception" << endl;
  }
  M::throw_ex = false;
  cout << endl;
}
