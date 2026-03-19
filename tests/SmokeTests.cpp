#include <cstdlib>
#include <iostream>

#include "cre/App.hpp"

namespace {

void require(bool condition, const char* message) {
    if (!condition) {
        std::cerr << "FAIL: " << message << "\n";
        std::exit(1);
    }
}

} // namespace

int main() {
    require(appcore::buildGreeting() == "cre ready", "greeting should match project bootstrap");
    std::cout << "OK\n";
    return 0;
}
