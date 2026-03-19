#include <exception>
#include <iostream>
#include <string_view>

#include "cre/App.hpp"
#include "cre/GuiApp.hpp"

namespace {

int runDemo() {
    std::cout << cre::renderExperimentReport(cre::runVirtualLabCycleDemo());
    return 0;
}

int runRecordRound(std::string_view outputDir) {
    if (outputDir.empty()) {
        std::cerr << "Usage: cre record-round <output-dir>\n";
        return 1;
    }

    const cre::RoundArtifacts artifacts = cre::recordVirtualLabRound(std::string(outputDir));
    std::cout << "Virtual lab round recorded:\n";
    std::cout << "- case: " << artifacts.casePath.string() << "\n";
    std::cout << "- hypothesis: " << artifacts.hypothesisPath.string() << "\n";
    std::cout << "- experiment: " << artifacts.experimentPath.string() << "\n";
    std::cout << "- evidence: " << artifacts.evidencePath.string() << "\n";
    return 0;
}

} // namespace

int main(int argc, char** argv) {
    try {
        if (argc <= 1) return runDemo();

        const std::string_view command = argv[1];
        if (command == "demo") return runDemo();
        if (command == "gui") return cre::runGuiApp();
        if (command == "record-round") {
            return runRecordRound(argc >= 3 ? std::string_view(argv[2]) : std::string_view{});
        }

        std::cerr << "Unknown command: " << command << "\n";
        std::cerr << "Usage: cre [demo|gui|record-round <output-dir>]\n";
        return 1;
    } catch (const std::exception& ex) {
        std::cerr << "CRE error: " << ex.what() << "\n";
        return 1;
    }
}
