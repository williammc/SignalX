#include "signalx/signalx.h"

#include <iostream>
#include <vector>

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
};

bool slot4(const char* e, std::size_t n) {
  std::cout << e << " [on line: " << n << "]" << std::endl;
  return false;
}

int main() {
  Sample sample;

  // Declare sigx::Signals using function signature syntax
  sigx::Signal<bool(const char*)> signal1;
  sigx::Signal<bool(const char*, std::size_t)> signal2;

  // Connect member functions to sigx::Signals
  signal1.connect<Sample, &Sample::slot1>(&sample);
  signal2.connect<Sample, &Sample::slot2>(&sample);

  // Connect a static member function
  signal1.connect<Sample::slot3>();

  // Connect a free function
  signal2.connect<slot4>();

  // Emit Signals
  signal1("signal 1");
  signal2("signal 2", __LINE__);

  std::vector<bool> status;

  // Emit Signals and accumulate SRVs (signal return values)
  signal1("signal 1 again", [&](bool srv) {
    status.push_back(srv);
  });

  // Disconnect member functions from a sigx::Signal
  signal1.disconnect<Sample, &Sample::slot1>(sample);
  signal2.disconnect<Sample, &Sample::slot2>(sample);

  // Disconnect a static member function
  signal1.disconnect<Sample::slot3>();

  // Disconnect a free function
  signal2.disconnect<slot4>();

  // Emit again to test disconnects
  signal1("THIS SHOULD NOT APPEAR");
  signal2("THIS SHOULD NOT APPEAR", __LINE__);

  // Pause the screen
  std::cin.get();
}