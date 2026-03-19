#pragma once

#include <cstddef>
#include <string>
#include <vector>

namespace appcore {

struct Scenario {
    std::string id;
    std::string title;
    std::string context;
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
    std::string source;
    std::string observation;
    double confidence{0.0};
};

class Experiment {
public:
    Experiment(Scenario scenario, Hypothesis hypothesis);

    void addEvidence(Evidence evidence);
    bool execute();

    [[nodiscard]] const Scenario& scenario() const;
    [[nodiscard]] const Hypothesis& hypothesis() const;
    [[nodiscard]] const std::vector<Evidence>& evidences() const;
    [[nodiscard]] bool wasExecuted() const;
    [[nodiscard]] bool supportsHypothesis() const;
    [[nodiscard]] double averageConfidence() const;

private:
    Scenario m_scenario;
    Hypothesis m_hypothesis;
    std::vector<Evidence> m_evidences;
    bool m_executed{false};
};

struct ExperimentReport {
    std::string scenarioId;
    bool hypothesisSupported{false};
    std::size_t evidenceCount{0};
    double averageConfidence{0.0};
    std::string summary;
};

std::string buildGreeting();
ExperimentReport runVirtualLabFlowDemo();

} // namespace appcore
