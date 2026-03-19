#include <cmath>
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

bool approxEqual(double a, double b, double eps = 1e-9) {
    return std::fabs(a - b) <= eps;
}

} // namespace

int main() {
    require(appcore::buildGreeting() == "cre virtual lab ready", "greeting should match project scope");

    const appcore::ExperimentReport report = appcore::runVirtualLabFlowDemo();
    require(report.scenarioId == "cenario-solo-001", "scenario id should be stable");
    require(report.evidenceCount == 2, "demo should register two evidences");
    require(report.hypothesisSupported, "hypothesis should be supported in demo flow");
    require(approxEqual(report.averageConfidence, 0.79), "average confidence should match demo data");
    require(!report.summary.empty(), "summary should be generated");

    std::cout << "OK\n";
    return 0;
}
