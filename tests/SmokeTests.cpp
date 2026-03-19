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

    const fs::path projectRoot = cre::findProjectRoot(fs::current_path());
    require(fs::exists(projectRoot / "README.md"), "project root should resolve from nested path");

    const fs::path tempWorkspace = fs::temp_directory_path() / "cre-workspace-smoke";
    fs::remove_all(tempWorkspace, ec);
    const cre::WorkspaceLayout layout =
        cre::resolveWorkspaceLayout(projectRoot, tempWorkspace, "lv-round-777");
    cre::ensureWorkspaceInitialized(layout);

    require(fs::exists(layout.workspaceRoot / "cre.yaml"), "workspace manifest should be created");
    require(fs::exists(layout.workspaceRoot / "rounds"), "workspace rounds directory should exist");
    require(layout.activeRoundRoot.filename() == "lv-round-777", "workspace should honor custom round id");

    const cre::RoundArtifacts workspaceArtifacts = cre::recordVirtualLabRoundInWorkspace(layout);
    require(fs::exists(workspaceArtifacts.casePath), "workspace case artifact should be written");
    require(fs::exists(workspaceArtifacts.hypothesisPath), "workspace hypothesis artifact should be written");
    require(fs::exists(workspaceArtifacts.experimentPath), "workspace experiment artifact should be written");
    require(fs::exists(workspaceArtifacts.evidencePath), "workspace evidence artifact should be written");

    const auto candidateDirs = cre::candidateRoundDirectories(layout);
    require(!candidateDirs.empty(), "workspace should expose candidate round directories");
    require(candidateDirs.front() == layout.activeRoundRoot, "workspace active round should be first candidate");

    fs::remove_all(tempDir, ec);
    fs::remove_all(tempWorkspace, ec);

    std::cout << "OK\n";
    return 0;
}
