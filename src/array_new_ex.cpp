#include <iostream>

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
  A *pA{new A[3]};
  B *pB{new B[3]};
  // Following lines will not be executed as an exception will occur.
  // Hence, pA memory will leak.
  delete[] pA;
  delete[] pB;
}

int main() {
  B::throwEx = true;
  try {
    alloc_A_and_B();
  } catch (Ex &) {
    cout << "caught exception" << endl;
  }
}
