#pragma once

#include <cstddef>
#include <filesystem>
#include <string>
#include <vector>

namespace cre {

struct LabCase {
    std::string id;
    std::string title;
    std::string context;
    std::string automaticRejection;
    std::string trigger;
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
    Experiment(LabCase labCase, Hypothesis hypothesis);

    void addEvidence(Evidence evidence);
    bool execute();

    [[nodiscard]] const LabCase& labCase() const;
    [[nodiscard]] const Hypothesis& hypothesis() const;
    [[nodiscard]] const std::vector<Evidence>& evidences() const;
    [[nodiscard]] bool wasExecuted() const;
    [[nodiscard]] bool supportsHypothesis() const;
    [[nodiscard]] double averageConfidence() const;

private:
    LabCase m_labCase;
    Hypothesis m_hypothesis;
    std::vector<Evidence> m_evidences;
    bool m_executed{false};
};

struct VirtualLabReport {
    std::string caseId;
    bool hypothesisSupported{false};
    std::size_t evidenceCount{0};
    double averageConfidence{0.0};
    std::string summary;
};

struct RoundArtifacts {
    std::filesystem::path casePath;
    std::filesystem::path hypothesisPath;
    std::filesystem::path experimentPath;
    std::filesystem::path evidencePath;
};

[[nodiscard]] std::string buildLabStatusMessage();
[[nodiscard]] Experiment createVirtualLabCycleDemo();
[[nodiscard]] VirtualLabReport runVirtualLabCycleDemo();
[[nodiscard]] std::string renderExperimentReport(const VirtualLabReport& report);
[[nodiscard]] RoundArtifacts recordVirtualLabRound(const std::filesystem::path& outputDir);

} // namespace cre
