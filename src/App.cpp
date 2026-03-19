#include "cre/App.hpp"

#include <fstream>
#include <sstream>
#include <stdexcept>
#include <utility>

namespace fs = std::filesystem;

namespace cre {
namespace {

std::string renderCaseArtifact(const LabCase& labCase) {
    std::ostringstream out;
    out << "# Virtual Lab Case\n\n";
    out << "- case_id: `" << labCase.id << "`\n";
    out << "- title: " << labCase.title << "\n";
    out << "- context: " << labCase.context << "\n";
    out << "- automatic_rejection: " << labCase.automaticRejection << "\n";
    out << "- trigger: " << labCase.trigger << "\n";
    return out.str();
}

std::string renderHypothesisArtifact(const std::string& caseId, const Hypothesis& hypothesis) {
    std::ostringstream out;
    out << "# Virtual Lab Hypothesis\n\n";
    out << "- related_case_id: `" << caseId << "`\n";
    out << "- statement: " << hypothesis.statement() << "\n";
    out << "- expected_improvement_pct: " << hypothesis.expectedImprovementPct() << "\n";
    out << "- testable: " << (hypothesis.isTestable() ? "yes" : "no") << "\n";
    return out.str();
}

std::string renderExperimentArtifact(const VirtualLabReport& report) {
    std::ostringstream out;
    out << "# Virtual Lab Experiment\n\n";
    out << "- case_id: `" << report.caseId << "`\n";
    out << "- evidence_count: " << report.evidenceCount << "\n";
    out << "- average_confidence: " << report.averageConfidence << "\n";
    out << "- hypothesis_supported: " << (report.hypothesisSupported ? "yes" : "no") << "\n";
    out << "- summary: " << report.summary << "\n";
    return out.str();
}

std::string renderEvidenceArtifact(const std::vector<Evidence>& evidences) {
    std::ostringstream out;
    out << "# Virtual Lab Evidence Log\n\n";
    for (const auto& evidence : evidences) {
        out << "## " << evidence.id << "\n";
        out << "- source: " << evidence.source << "\n";
        out << "- observation: " << evidence.observation << "\n";
        out << "- confidence: " << evidence.confidence << "\n\n";
    }
    return out.str();
}

void writeTextFile(const fs::path& path, const std::string& content) {
    std::ofstream out(path);
    if (!out.is_open()) {
        throw std::runtime_error("failed to open output file: " + path.string());
    }
    out << content;
    if (!out.good()) {
        throw std::runtime_error("failed to write output file: " + path.string());
    }
}

Experiment buildVirtualLabExperiment() {
    LabCase labCase{
        .id = "LV-CASE-101",
        .title = "Introduzir o CRE sem ativar rejeicao por complexidade",
        .context = "Laboratorio Virtual CRE: validar uma entrada por caso concreto antes da teoria completa.",
        .automaticRejection = "Isso parece complexo demais para valer o teste.",
        .trigger = "Excesso de abstracao e linguagem tecnica logo na entrada."
    };

    Hypothesis hypothesis{
        "Se a abertura iniciar por uma demonstracao concreta de uso, entao a rejeicao inicial reduz porque o valor aparece antes da carga conceitual.",
        15.0
    };

    Experiment experiment{labCase, hypothesis};
    experiment.addEvidence({
        .id = "LV-EVD-101",
        .source = "simulacao-de-apresentacao-v1",
        .observation = "A demonstracao curta gerou perguntas de uso em vez de resistencia inicial.",
        .confidence = 0.76
    });
    experiment.addEvidence({
        .id = "LV-EVD-102",
        .source = "observacao-de-rodada-piloto",
        .observation = "O valor percebido apareceu antes da necessidade de explicar o sistema inteiro.",
        .confidence = 0.82
    });
    experiment.execute();
    return experiment;
}

} // namespace

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

Experiment::Experiment(LabCase labCase, Hypothesis hypothesis)
    : m_labCase(std::move(labCase)), m_hypothesis(std::move(hypothesis)) {
}

void Experiment::addEvidence(Evidence evidence) {
    m_evidences.push_back(std::move(evidence));
}

bool Experiment::execute() {
    m_executed = m_hypothesis.isTestable() && !m_evidences.empty();
    return m_executed;
}

const LabCase& Experiment::labCase() const {
    return m_labCase;
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

std::string buildLabStatusMessage() {
    return "CRE Laboratorio Virtual pronto";
}

Experiment createVirtualLabCycleDemo() {
    return buildVirtualLabExperiment();
}

VirtualLabReport runVirtualLabCycleDemo() {
    const Experiment experiment = createVirtualLabCycleDemo();

    VirtualLabReport report;
    report.caseId = experiment.labCase().id;
    report.hypothesisSupported = experiment.supportsHypothesis();
    report.evidenceCount = experiment.evidences().size();
    report.averageConfidence = experiment.averageConfidence();

    std::ostringstream summary;
    summary << "Caso " << report.caseId
            << ": hipotese "
            << (report.hypothesisSupported ? "corroborada" : "nao corroborada")
            << " com " << report.evidenceCount
            << " evidencias e confianca media " << report.averageConfidence;
    report.summary = summary.str();
    return report;
}

std::string renderExperimentReport(const VirtualLabReport& report) {
    std::ostringstream out;
    out << buildLabStatusMessage() << "\n";
    out << report.summary << "\n";
    return out.str();
}

RoundArtifacts recordVirtualLabRound(const fs::path& outputDir) {
    const Experiment experiment = createVirtualLabCycleDemo();
    const VirtualLabReport report = runVirtualLabCycleDemo();

    std::error_code ec;
    fs::create_directories(outputDir, ec);
    if (ec) {
        throw std::runtime_error("failed to create output directory: " + outputDir.string());
    }

    RoundArtifacts artifacts{
        .casePath = outputDir / "LV-CASE-101.md",
        .hypothesisPath = outputDir / "LV-HYP-101.md",
        .experimentPath = outputDir / "LV-EXP-101.md",
        .evidencePath = outputDir / "LV-EVD-LOG-101.md"
    };

    writeTextFile(artifacts.casePath, renderCaseArtifact(experiment.labCase()));
    writeTextFile(artifacts.hypothesisPath, renderHypothesisArtifact(experiment.labCase().id, experiment.hypothesis()));
    writeTextFile(artifacts.experimentPath, renderExperimentArtifact(report));
    writeTextFile(artifacts.evidencePath, renderEvidenceArtifact(experiment.evidences()));

    return artifacts;
}

} // namespace cre
