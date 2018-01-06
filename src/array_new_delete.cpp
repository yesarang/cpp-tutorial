#include <iostream>

using namespace std;

struct A {
  A() { cout << __FUNCTION__ << " called for " << id_ << endl; }

  ~A() { cout << __FUNCTION__ << " called for " << id_ << endl; }

  static int id;
  int id_ = id++;
};

int A::id = 0;

int main() {
  A *pA{new A[3]};

  delete[] pA;
}
