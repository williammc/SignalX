#include "signalx/signalx.h"
#include <memory>
#include <iostream>
#include <vector>

struct Foo {
  Foo() { a = 1; }
  int a;
};

/// deriving Observer for automatic disconnection management
struct Sample /*: public sigx::Observer*/ {
  bool slot1(const char* e) const {
    std::cout << e << std::endl;
    return true;
  }

  bool slot2(const char* e, std::size_t n) {
    std::cout << e << " [on line: " << n << "]" << std::endl;
    return true;
  }

  static bool slot3(const char* e) {
    std::cout << e << std::endl;
    return true;
  }

  static void slot4_1(std::shared_ptr<Foo> f) {
    printf("Foo:%d\n", f->a);
  }
};

void slot4_2(std::shared_ptr<Foo> f) {
  printf("Foo:%d\n", f->a);
}

bool slot5(const char* e, std::size_t n) {
  std::cout << e << " [on line: " << n << "]" << std::endl;
  return false;
}

int main() {
  Sample sample;

  // Declare sigx::Signals using function signature syntax
  sigx::Signal<bool(const char*)> signal1;
  sigx::Signal<bool(const char*, std::size_t)> signal2;
  sigx::Signal<bool(const char*, std::size_t, int)> signal3;
  sigx::Signal<void(std::shared_ptr<Foo>)> signal4;

  // Connect member functions to sigx::Signals
  signal1.connect<Sample, &Sample::slot1>(&sample);
  signal2.connect<Sample, &Sample::slot2>(&sample);

  // Connect a static member function
  signal1.connect<Sample::slot3>();

  // Connect a free function
  signal2.connect<slot5>();

  signal4.connect<slot4_2>();
  signal4.connect<Sample::slot4_1>();

  // Emit Signals
  signal1("signal 1");
  signal2("signal 2", __LINE__);
  auto f = std::make_shared<Foo>();
  signal4(f);

  std::vector<bool> status;

  // Emit Signals and accumulate SRVs (signal return values)
  signal1("signal 1 again modified", [&](bool srv) {
    status.push_back(srv);
  });

  // Disconnect member functions from a sigx::Signal
  signal1.disconnect<Sample, &Sample::slot1>(sample);
  signal2.disconnect<Sample, &Sample::slot2>(sample);

  // Disconnect a static member function
  signal1.disconnect<Sample::slot3>();

  // Disconnect a free function
  signal2.disconnect<slot5>();

  signal4.disconnect<slot4_2>();
  signal4.disconnect<Sample::slot4_1>();

  // Emit again to test disconnects
  signal1("THIS SHOULD NOT APPEAR");
  signal2("THIS SHOULD NOT APPEAR", __LINE__);

  signal4(f);

  std::cin.get(); // Pause the screen
}