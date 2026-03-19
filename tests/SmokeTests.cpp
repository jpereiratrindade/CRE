#include <cmath>
#include <cstdlib>
#include <filesystem>
#include <iostream>

#include "cre/App.hpp"

namespace fs = std::filesystem;

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
    require(cre::buildLabStatusMessage() == "CRE Laboratorio Virtual pronto",
            "status message should reflect CRE virtual lab scope");

    const cre::VirtualLabReport report = cre::runVirtualLabCycleDemo();
    require(report.caseId == "LV-CASE-101", "case id should be stable");
    require(report.evidenceCount == 2, "demo should register two evidences");
    require(report.hypothesisSupported, "hypothesis should be supported in demo flow");
    require(approxEqual(report.averageConfidence, 0.79), "average confidence should match demo data");
    require(!report.summary.empty(), "summary should be generated");

    const fs::path tempDir = fs::temp_directory_path() / "cre-lv-round-output";
    std::error_code ec;
    fs::remove_all(tempDir, ec);
    const cre::RoundArtifacts artifacts = cre::recordVirtualLabRound(tempDir);

    require(fs::exists(artifacts.casePath), "case artifact should be written");
    require(fs::exists(artifacts.hypothesisPath), "hypothesis artifact should be written");
    require(fs::exists(artifacts.experimentPath), "experiment artifact should be written");
    require(fs::exists(artifacts.evidencePath), "evidence artifact should be written");

    fs::remove_all(tempDir, ec);

    std::cout << "OK\n";
    return 0;
}
