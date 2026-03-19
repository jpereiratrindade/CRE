#include <iostream>

#include "cre/App.hpp"

int main() {
    std::cout << appcore::buildGreeting() << "\n";

    const appcore::ExperimentReport report = appcore::runVirtualLabFlowDemo();
    std::cout << report.summary << "\n";

    return 0;
}
