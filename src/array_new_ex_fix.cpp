#include <iostream>
#include <memory>

using namespace std;

class Ex {};

struct A {
  A() { cout << __FUNCTION__ << " called for " << id_ << endl; }

  ~A() { cout << __FUNCTION__ << " called for " << id_ << endl; }

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

  ~B() { cout << __FUNCTION__ << " called for " << id_ << endl; }

  static int id;
  static bool throwEx;
  int id_ = id++;
};

int B::id = 0;
bool B::throwEx = false;

void alloc_A_and_B() {
  unique_ptr<A[]> pA{new A[3]};
  unique_ptr<B[]> pB{new B[3]};
  // Following lines will not be executed as an exception will occur.
  // While stack unwinding, pA's dtor will be called and what pA's dtor does is
  // delete[] pA.
  // In this way, pA's memory will not leak even in case of exception.
}

int main() {
  B::throwEx = true;
  try {
    alloc_A_and_B();
  } catch (Ex &) {
    cout << "caught exception" << endl;
  }
}
