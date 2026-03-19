#pragma once

#include <cstddef>
#include <filesystem>
#include <optional>
#include <string>
#include <string_view>
#include <vector>

namespace cre {

struct LabCase {
    std::string id;
    std::string title;
    std::string context;
    std::string automaticRejection;
    std::string trigger;
};

struct RejectionSignal {
    std::string heuristic;
    std::string observedPhrase;
};

struct ActivationTrigger {
    std::string primary;
    std::string secondary;
};

struct ReframingStrategy {
    std::string title;
    std::string entryShift;
    std::string expectedPerceptionShift;
};

struct ControlledExposure {
    std::string design;
    std::vector<std::string> steps;
    std::string guardrail;
};

struct ScenarioDescriptor {
    std::string id;
    std::string label;
    std::string description;
};

struct ReframingDraft {
    std::string roundId;
    std::string scenarioId;
    std::string scenarioLabel;
    std::string scenarioDescription;
    std::string caseId;
    std::string caseTitle;
    std::string caseContext;
    std::string automaticRejection;
    std::string latentRejection;
    std::string observedPhrase;
    std::string activationTriggerPrimary;
    std::string activationTriggerSecondary;
    std::string hypothesisStatement;
    double expectedImprovementPct{0.0};
    std::string reframingTitle;
    std::string reframingStrategy;
    std::string expectedPerceptionShift;
    std::string exposureDesign;
    std::string exposureGuardrail;
    std::string ethicalLimit;
};

class Hypothesis {
public:
    Hypothesis(std::string statement, double expectedImprovementPct);

    [[nodiscard]] const std::string& statement() const;
    [[nodiscard]] double expectedImprovementPct() const;
    [[nodiscard]] bool isTestable() const;

private:
    std::string m_statement;
    double m_expectedImprovementPct{0.0};
};

struct Evidence {
    std::string id;
    std::string source;
    std::string observation;
    double confidence{0.0};
};

class Experiment {
public:
    Experiment(
        LabCase labCase,
        RejectionSignal rejection,
        ActivationTrigger trigger,
        Hypothesis hypothesis,
        ReframingStrategy reframing,
        ControlledExposure exposure);

    void addEvidence(Evidence evidence);
    bool execute();

    [[nodiscard]] const LabCase& labCase() const;
    [[nodiscard]] const RejectionSignal& rejection() const;
    [[nodiscard]] const ActivationTrigger& activationTrigger() const;
    [[nodiscard]] const Hypothesis& hypothesis() const;
    [[nodiscard]] const ReframingStrategy& reframing() const;
    [[nodiscard]] const ControlledExposure& exposure() const;
    [[nodiscard]] const std::vector<Evidence>& evidences() const;
    [[nodiscard]] bool wasExecuted() const;
    [[nodiscard]] bool supportsHypothesis() const;
    [[nodiscard]] double averageConfidence() const;

private:
    LabCase m_labCase;
    RejectionSignal m_rejection;
    ActivationTrigger m_trigger;
    Hypothesis m_hypothesis;
    ReframingStrategy m_reframing;
    ControlledExposure m_exposure;
    std::vector<Evidence> m_evidences;
    bool m_executed{false};
};

struct PerceptionShiftReport {
    std::string caseId;
    std::string latentRejection;
    std::string activationTrigger;
    std::string reframingStrategy;
    std::string exposureDesign;
    bool hypothesisSupported{false};
    std::string decision;
    std::size_t evidenceCount{0};
    double averageConfidence{0.0};
    std::string summary;
};

using VirtualLabReport = PerceptionShiftReport;

struct RoundArtifacts {
    std::filesystem::path casePath;
    std::filesystem::path hypothesisPath;
    std::filesystem::path experimentPath;
    std::filesystem::path evidencePath;
    std::filesystem::path metadataPath;
};

struct WorkspaceLayout {
    std::filesystem::path projectRoot;
    std::filesystem::path workspaceRoot;
    std::filesystem::path manifestPath;
    std::filesystem::path roundsRoot;
    std::filesystem::path activeRoundRoot;
    std::filesystem::path legacyRoundRoot;
};

[[nodiscard]] std::string buildLabStatusMessage();
[[nodiscard]] std::vector<ScenarioDescriptor> listVirtualLabScenarios();
[[nodiscard]] std::string buildNextRoundId(const WorkspaceLayout& layout);
[[nodiscard]] std::string buildNextCaseId(const WorkspaceLayout& layout);
[[nodiscard]] Experiment createVirtualLabCycleDemo(std::string_view scenarioId = "cre-intro");
[[nodiscard]] VirtualLabReport runVirtualLabCycleDemo(std::string_view scenarioId = "cre-intro");
[[nodiscard]] std::string renderExperimentReport(const VirtualLabReport& report);
[[nodiscard]] RoundArtifacts recordVirtualLabRound(
    const std::filesystem::path& outputDir,
    std::string_view scenarioId = "cre-intro");
[[nodiscard]] std::filesystem::path findProjectRoot(const std::filesystem::path& startDir);
[[nodiscard]] WorkspaceLayout resolveWorkspaceLayout(
    const std::filesystem::path& startDir,
    const std::optional<std::filesystem::path>& explicitWorkspaceRoot = std::nullopt,
    std::string_view roundId = "lv-round-001");
void ensureWorkspaceInitialized(const WorkspaceLayout& layout);
[[nodiscard]] RoundArtifacts recordVirtualLabRoundInWorkspace(
    const WorkspaceLayout& layout,
    bool mirrorLegacyOutput = false,
    std::string_view scenarioId = "cre-intro");
[[nodiscard]] RoundArtifacts recordDraftRoundInWorkspace(
    const WorkspaceLayout& layout,
    const ReframingDraft& draft);
[[nodiscard]] std::vector<std::filesystem::path> listWorkspaceRounds(const WorkspaceLayout& layout);
[[nodiscard]] std::vector<std::filesystem::path> candidateRoundDirectories(const WorkspaceLayout& layout);

} // namespace cre
