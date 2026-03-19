#include <exception>
#include <filesystem>
#include <iostream>
#include <optional>
#include <string>
#include <string_view>

#include "cre/App.hpp"
#if CRE_ENABLE_GUI
#include "cre/GuiApp.hpp"
#endif

namespace {

struct ParsedOptions {
    std::optional<std::filesystem::path> workspaceRoot;
    std::string roundId{"lv-round-001"};
    std::string scenarioId{"cre-intro"};
};

int runDemo(const ParsedOptions& options) {
    std::cout << cre::renderExperimentReport(cre::runVirtualLabCycleDemo(options.scenarioId));
    return 0;
}

std::optional<ParsedOptions> parseOptions(int argc, char** argv, int startIndex) {
    ParsedOptions options;
    for (int i = startIndex; i < argc; ++i) {
        const std::string_view arg = argv[i];
        if (arg == "--workspace") {
            if (i + 1 >= argc) {
                std::cerr << "Missing value for --workspace\n";
                return std::nullopt;
            }
            options.workspaceRoot = std::filesystem::path(argv[++i]);
            continue;
        }
        if (arg == "--round-id") {
            if (i + 1 >= argc) {
                std::cerr << "Missing value for --round-id\n";
                return std::nullopt;
            }
            options.roundId = argv[++i];
            continue;
        }
        if (arg == "--scenario") {
            if (i + 1 >= argc) {
                std::cerr << "Missing value for --scenario\n";
                return std::nullopt;
            }
            options.scenarioId = argv[++i];
            continue;
        }
        std::cerr << "Unknown option: " << arg << "\n";
        return std::nullopt;
    }
    return options;
}

int runRecordRound(const ParsedOptions& options) {
    const cre::WorkspaceLayout layout =
        cre::resolveWorkspaceLayout(std::filesystem::current_path(), options.workspaceRoot, options.roundId);
    const cre::RoundArtifacts artifacts = cre::recordVirtualLabRoundInWorkspace(layout, false, options.scenarioId);
    std::cout << "Virtual lab round recorded:\n";
    std::cout << "- scenario: " << options.scenarioId << "\n";
    std::cout << "- workspace: " << layout.workspaceRoot.string() << "\n";
    std::cout << "- round: " << layout.activeRoundRoot.string() << "\n";
    std::cout << "- case: " << artifacts.casePath.string() << "\n";
    std::cout << "- hypothesis: " << artifacts.hypothesisPath.string() << "\n";
    std::cout << "- experiment: " << artifacts.experimentPath.string() << "\n";
    std::cout << "- evidence: " << artifacts.evidencePath.string() << "\n";
    std::cout << "- metadata: " << artifacts.metadataPath.string() << "\n";
    return 0;
}

} // namespace

int main(int argc, char** argv) {
    try {
        if (argc <= 1) {
#if CRE_ENABLE_GUI
            return cre::runGuiApp();
#else
            return runDemo(ParsedOptions{});
#endif
        }

        const std::string_view command = argv[1];
        if (command == "demo") {
            const auto options = parseOptions(argc, argv, 2);
            if (!options.has_value()) {
                std::cerr << "Usage: cre demo [--scenario <id>]\n";
                return 1;
            }
            return runDemo(*options);
        }
        if (command == "gui") {
#if CRE_ENABLE_GUI
            return cre::runGuiApp();
#else
            std::cerr << "GUI indisponivel neste build (cre_ENABLE_GUI=OFF)\n";
            return 1;
#endif
        }
        if (command == "record-round") {
            const auto options = parseOptions(argc, argv, 2);
            if (!options.has_value()) {
                std::cerr << "Usage: cre record-round [--workspace <path>] [--round-id <id>] [--scenario <id>]\n";
                return 1;
            }
            return runRecordRound(*options);
        }

        std::cerr << "Unknown command: " << command << "\n";
        std::cerr << "Usage: cre [demo|gui|record-round [--workspace <path>] [--round-id <id>] [--scenario <id>]]\n";
        return 1;
    } catch (const std::exception& ex) {
        std::cerr << "CRE error: " << ex.what() << "\n";
        return 1;
    }
}
