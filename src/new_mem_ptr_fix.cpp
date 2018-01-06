#include <iostream>
#include <memory>

using namespace std;

class Ex {};

// base class #1
struct B1 {
  B1() { cout << __FUNCTION__ << " called for " << id_ << endl; }

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

  ~B2() {
    cout << __FUNCTION__ << " called for " << id_ << endl;
    cout.flush();
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
    cout << __FUNCTION__ << " called for (" << B1::id_ << "," << B2::id_ << ")"
         << endl;
  }

  ~D() {
    cout << __FUNCTION__ << " called for (" << B1::id_ << "," << B2::id_ << ")"
         << endl;
    cout.flush();
  }
};

struct E1 {
  E1() : pD_{nullptr}, pM_{nullptr}, initialized_{false} {
    cout << __FUNCTION__ << " called" << endl;
    cout.flush();
  }

  void initialize() {
    cout << __FUNCTION__ << " called" << endl;
    cout.flush();

    try {
      pD_ = new D();
      pM_ = new M();
      initialized_ = true;

      cout << __FUNCTION__ << " called for (" << pD_->B1::id_ << ","
           << pD_->B2::id_ << ")" << endl;
      cout.flush();
    } catch (...) {
      cout << __FUNCTION__ << " caught the exception" << endl;
      cout.flush();

      delete pM_;
      pM_ = nullptr;
      delete pD_;
      pD_ = nullptr;
      throw;
    }
  }

  bool initialized() const { return initialized_; }

  int getB1Id() const { return pD_->B1::id_; }

  ~E1() {
    if (initialized_) {
      cout << __FUNCTION__ << " called for (" << pD_->B1::id_ << ","
           << pD_->B2::id_ << ")" << endl;
    } else {
      cout << __FUNCTION__ << " called" << endl;
    }
    cout.flush();
    if (initialized_) {
      delete pM_;
      delete pD_;
    }
  }

  D *pD_;
  M *pM_;
  bool initialized_;
};

struct E2 {
  E2() : pD_{nullptr}, pM_{nullptr}, initialized_{false} {
    cout << __FUNCTION__ << " called" << endl;
    cout.flush();
  }

  void initialize() {
    cout << __FUNCTION__ << " called" << endl;
    cout.flush();

    unique_ptr<D> pD{new D()};
    unique_ptr<M> pM{new M()};

    pD_ = pD.release();
    pM_ = pM.release();
    initialized_ = true;

    cout << __FUNCTION__ << " called for (" << pD_->B1::id_ << ","
         << pD_->B2::id_ << ")" << endl;
    cout.flush();
  }

  bool initialized() const { return initialized_; }

  int getB1Id() const { return pD_->B1::id_; }

  ~E2() {
    if (initialized_) {
      cout << __FUNCTION__ << " called for (" << pD_->B1::id_ << ","
           << pD_->B2::id_ << ")" << endl;
    } else {
      cout << __FUNCTION__ << " called" << endl;
    }
    cout.flush();
    if (initialized_) {
      delete pM_;
      delete pD_;
    }
  }

  D *pD_;
  M *pM_;
  bool initialized_;
};

// a class which has a member pointer to a class
struct E {
  E() : pD_{new D()}, pM_{new M()} {
    cout << __FUNCTION__ << " called for (" << pD_->B1::id_ << ","
         << pD_->B2::id_ << ")" << endl;
    cout.flush();
  }

  ~E() {
    cout << __FUNCTION__ << " called for (" << pD_->B1::id_ << ","
         << pD_->B2::id_ << ")" << endl;
    cout.flush();
  }

  int getB1Id() const { return pD_->B1::id_; }

  unique_ptr<D> pD_;
  unique_ptr<M> pM_;
};

void new_mem_ptr_naive_fix() {
  // Naive fix for case #4:
  cout << "Naive fix for case #4: " << endl;
  E1 *pE1 = new E1();
  try {
    pE1->initialize();
  } catch (Ex &) {
    cout << "caught an exception for E1" << endl;
  }
  if (pE1->initialized()) {
    cout << "id of pE1 = " << pE1->getB1Id() << endl;
  }
  delete pE1;
  cout << endl;
}

void new_mem_ptr_naive_fix2() {
  // Naive fix 2 for case #4:
  cout << "Naive fix 2 for case #4: " << endl;
  E2 *pE2 = new E2();
  try {
    pE2->initialize();
  } catch (Ex &) {
    cout << "caught an exception for E2" << endl;
  }
  if (pE2->initialized()) {
    cout << "id of pE2 = " << pE2->getB1Id() << endl;
  }
  delete pE2;
  cout << endl;
}

void new_mem_ptr_good_fix() {
  // Final fix for case #4:
  // This shows what will happen when an exception is thrown during construction
  // M will throw an exception
  // An object which is pointed to by a member pointer will be properly
  // destroyed by unique_ptr
  cout << "Final fix for case #4: " << endl;
  unique_ptr<E> pE{make_unique<E>()};
  cout << "id of pE = " << pE->getB1Id() << endl;
}

int main() {
  M::throw_ex = true;

  new_mem_ptr_naive_fix();

  new_mem_ptr_naive_fix2();

  try {
    new_mem_ptr_good_fix();
  } catch (...) {
    cout << "caught an exception for E" << endl;
  }

  M::throw_ex = false;
}
