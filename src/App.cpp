#include "cre/App.hpp"

#include <sstream>
#include <utility>

namespace appcore {

Hypothesis::Hypothesis(std::string statement, double expectedImprovementPct)
    : m_statement(std::move(statement)), m_expectedImprovementPct(expectedImprovementPct) {
}

const std::string& Hypothesis::statement() const {
    return m_statement;
}

double Hypothesis::expectedImprovementPct() const {
    return m_expectedImprovementPct;
}

bool Hypothesis::isTestable() const {
    return !m_statement.empty() && m_expectedImprovementPct > 0.0;
}

Experiment::Experiment(Scenario scenario, Hypothesis hypothesis)
    : m_scenario(std::move(scenario)), m_hypothesis(std::move(hypothesis)) {
}

void Experiment::addEvidence(Evidence evidence) {
    m_evidences.push_back(std::move(evidence));
}

bool Experiment::execute() {
    m_executed = m_hypothesis.isTestable() && !m_evidences.empty();
    return m_executed;
}

const Scenario& Experiment::scenario() const {
    return m_scenario;
}

const Hypothesis& Experiment::hypothesis() const {
    return m_hypothesis;
}

const std::vector<Evidence>& Experiment::evidences() const {
    return m_evidences;
}

bool Experiment::wasExecuted() const {
    return m_executed;
}

double Experiment::averageConfidence() const {
    if (m_evidences.empty()) return 0.0;
    double sum = 0.0;
    for (const auto& evidence : m_evidences) {
        sum += evidence.confidence;
    }
    return sum / static_cast<double>(m_evidences.size());
}

bool Experiment::supportsHypothesis() const {
    return m_executed && averageConfidence() >= 0.70;
}

std::string buildGreeting() {
    return "cre virtual lab ready";
}

ExperimentReport runVirtualLabFlowDemo() {
    Scenario scenario{
        .id = "cenario-solo-001",
        .title = "Erosao em area experimental",
        .context = "Laboratorio Virtual CRE: analisar mitigacao com cobertura vegetal"
    };

    Hypothesis hypothesis{
        "Cobertura vegetal reduz erosao observada em comparacao ao controle",
        15.0
    };

    Experiment experiment{scenario, hypothesis};
    experiment.addEvidence({
        .source = "simulacao-hidrica-v1",
        .observation = "queda de 18% na perda de solo",
        .confidence = 0.76
    });
    experiment.addEvidence({
        .source = "serie-historica-parcela-a",
        .observation = "reducoes consistentes em eventos de chuva intensa",
        .confidence = 0.82
    });

    experiment.execute();

    ExperimentReport report;
    report.scenarioId = experiment.scenario().id;
    report.hypothesisSupported = experiment.supportsHypothesis();
    report.evidenceCount = experiment.evidences().size();
    report.averageConfidence = experiment.averageConfidence();

    std::ostringstream oss;
    oss << "Scenario " << report.scenarioId
        << ": hypothesis "
        << (report.hypothesisSupported ? "supported" : "not supported")
        << " with " << report.evidenceCount
        << " evidences and avg confidence " << report.averageConfidence;
    report.summary = oss.str();

    return report;
}

} // namespace appcore
