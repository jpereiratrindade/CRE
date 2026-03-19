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

    const auto scenarios = cre::listVirtualLabScenarios();
    require(scenarios.size() >= 3, "CRE should expose multiple built-in scenarios");

    const cre::VirtualLabReport report = cre::runVirtualLabCycleDemo();
    require(report.caseId == "LV-CASE-101", "case id should be stable");
    require(report.latentRejection == "complexidade alta = nao vale testar",
            "report should expose the latent rejection heuristic");
    require(report.activationTrigger == "Excesso de abstracao e linguagem tecnica logo na entrada.",
            "report should expose the primary activation trigger");
    require(report.reframingStrategy == "Trocar abertura abstrata por demonstracao curta de uso real.",
            "report should expose the reframing strategy");
    require(report.exposureDesign == "Demonstracao curta antes da explicacao arquitetural.",
            "report should expose the exposure design");
    require(report.evidenceCount == 2, "demo should register two evidences");
    require(report.hypothesisSupported, "hypothesis should be supported in demo flow");
    require(report.decision == "promover", "supported hypothesis should recommend promotion");
    require(approxEqual(report.averageConfidence, 0.79), "average confidence should match demo data");
    require(!report.summary.empty(), "summary should be generated");

    const cre::VirtualLabReport educationReport = cre::runVirtualLabCycleDemo("education-inclusion");
    require(educationReport.caseId == "LV-CASE-201", "education scenario should be available");
    require(educationReport.hypothesisSupported, "education scenario should be supported");

    const cre::VirtualLabReport strataReport = cre::runVirtualLabCycleDemo("strata-entry");
    require(strataReport.caseId == "LV-CASE-301", "STRATA scenario should be available");
    require(strataReport.hypothesisSupported, "STRATA scenario should be supported");

    const fs::path tempDir = fs::temp_directory_path() / "cre-lv-round-output";
    std::error_code ec;
    fs::remove_all(tempDir, ec);
    const cre::RoundArtifacts artifacts = cre::recordVirtualLabRound(tempDir, "education-inclusion");

    require(fs::exists(artifacts.casePath), "case artifact should be written");
    require(fs::exists(artifacts.hypothesisPath), "hypothesis artifact should be written");
    require(fs::exists(artifacts.experimentPath), "experiment artifact should be written");
    require(fs::exists(artifacts.evidencePath), "evidence artifact should be written");
    require(fs::exists(artifacts.metadataPath), "round metadata artifact should be written");
    require(artifacts.casePath.filename() == "LV-CASE-201.md", "case artifact filename should match scenario case id");
    require(artifacts.hypothesisPath.filename() == "LV-HYP-201.md", "hypothesis artifact filename should match scenario case id");

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
    require(fs::exists(workspaceArtifacts.metadataPath), "workspace metadata artifact should be written");

    const cre::WorkspaceLayout laterLayout =
        cre::resolveWorkspaceLayout(projectRoot, tempWorkspace, "lv-round-888");
    const cre::RoundArtifacts laterArtifacts = cre::recordVirtualLabRoundInWorkspace(laterLayout);
    require(fs::exists(laterArtifacts.casePath), "later workspace case artifact should be written");
    require(fs::exists(laterArtifacts.metadataPath), "later workspace metadata artifact should be written");

    const auto candidateDirs = cre::candidateRoundDirectories(layout);
    require(!candidateDirs.empty(), "workspace should expose candidate round directories");
    require(candidateDirs.front() == layout.activeRoundRoot, "workspace active round should be first candidate");

    const auto roundDirs = cre::listWorkspaceRounds(layout);
    require(roundDirs.size() >= 2, "workspace should list recorded rounds");
    require(roundDirs[0].filename() == "lv-round-888", "most recent round should sort first");
    require(roundDirs[1].filename() == "lv-round-777", "older round should remain available");

    const cre::WorkspaceLayout draftLayout =
        cre::resolveWorkspaceLayout(projectRoot, tempWorkspace, "lv-round-999");
    cre::ReframingDraft draft;
    draft.roundId = "lv-round-999";
    draft.scenarioId = "custom-education";
    draft.scenarioLabel = "Custom Education";
    draft.scenarioDescription = "Draft manual criado pela GUI.";
    draft.caseId = "LV-CASE-999";
    draft.caseTitle = "Caso manual";
    draft.caseContext = "Contexto manual";
    draft.automaticRejection = "Resistencia inicial";
    draft.latentRejection = "heuristica manual";
    draft.observedPhrase = "frase manual";
    draft.activationTriggerPrimary = "gatilho principal";
    draft.activationTriggerSecondary = "gatilho secundario";
    draft.hypothesisStatement = "Hipotese manual";
    draft.expectedImprovementPct = 12.0;
    draft.reframingTitle = "Reframing manual";
    draft.reframingStrategy = "Trocar entrada";
    draft.expectedPerceptionShift = "Curiosidade";
    draft.exposureDesign = "Exposicao controlada";
    draft.exposureGuardrail = "Guardrail";
    draft.ethicalLimit = "Limite etico";

    const cre::RoundArtifacts draftArtifacts = cre::recordDraftRoundInWorkspace(draftLayout, draft);
    require(fs::exists(draftArtifacts.casePath), "draft case artifact should be written");
    require(fs::exists(draftArtifacts.metadataPath), "draft metadata artifact should be written");

    fs::remove_all(tempDir, ec);
    fs::remove_all(tempWorkspace, ec);

    std::cout << "OK\n";
    return 0;
}
