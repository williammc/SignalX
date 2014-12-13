#include "signalx/signalx.h"

#include <iostream>
#include <vector>

struct Foo /*: public sigx::Observer*/ {
    bool handler_a(const char* e) const {
        std::cout << e << std::endl;
        return true;
    }

    bool handler_b(const char* e, std::size_t n) {
        std::cout << e << " [on line: " << n << "]" << std::endl;
        return true;
    }

    static bool handler_c(const char* e) {
        std::cout << e << std::endl;
        return true;
    }
};

bool handler_d(const char* e, std::size_t n) {
    std::cout << e << " [on line: " << n << "]" << std::endl;
    return false;
}

int main() {
    Foo foo;

    // Declare sigx::Signals using function signature syntax
    sigx::Signal<bool(const char*)> signal1;
    sigx::Signal<bool(const char*, std::size_t)> signal2;

    // Connect member functions to sigx::Signals
    signal1.connect<Foo, &Foo::handler_a>(&foo);
    signal2.connect<Foo, &Foo::handler_b>(&foo);

    // Connect a static member function
    signal1.connect<Foo::handler_c>();

    // Connect a free function
    signal2.connect<handler_d>();

    // Emit Signals
    signal1("signal 1");
    signal2("signal 2", __LINE__);

    std::vector<bool> status;

    // Emit Signals and accumulate SRVs (signal return values)
    signal1("signal 1 again", [&](bool srv) {
        status.push_back(srv);
    });

    // Disconnect member functions from a sigx::Signal
    signal1.disconnect<Foo, &Foo::handler_a>(foo);
    signal2.disconnect<Foo, &Foo::handler_b>(foo);

    // Disconnect a static member function
    signal1.disconnect<Foo::handler_c>();

    // Disconnect a free function
    signal2.disconnect<handler_d>();

    // Emit again to test disconnects
    signal1("THIS SHOULD NOT APPEAR");
    signal2("THIS SHOULD NOT APPEAR", __LINE__);

    // Pause the screen
    std::cin.get();
}