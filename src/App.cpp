#include "cre/App.hpp"

#include <algorithm>
#include <cstdlib>
#include <fstream>
#include <optional>
#include <sstream>
#include <stdexcept>
#include <system_error>
#include <utility>

namespace fs = std::filesystem;

namespace cre {
namespace {

constexpr std::string_view kDefaultRoundId = "lv-round-001";

std::optional<fs::path> tryResolveManifestRoot(const fs::path& startDir) {
    std::error_code ec;
    fs::path current = fs::absolute(startDir, ec);
    if (ec) return std::nullopt;

    while (!current.empty()) {
        if (fs::exists(current / "cre.yaml")) return current;
        const fs::path parent = current.parent_path();
        if (parent == current) break;
        current = parent;
    }
    return std::nullopt;
}

std::string buildWorkspaceManifest() {
    return std::string{}
        + "workspace_id: cre-default\n"
        + "version: 1\n"
        + "defaults:\n"
        + "  active_round: " + std::string(kDefaultRoundId) + "\n"
        + "paths:\n"
        + "  rounds: rounds\n"
        + "  cases: cases\n"
        + "  hypotheses: hypotheses\n"
        + "  experiments: experiments\n"
        + "  evidences: evidences\n"
        + "  cycles: cycles\n"
        + "  logs: logs\n"
        + "  exports: exports\n";
}

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

std::string renderHypothesisArtifact(const std::string& caseId,
                                     const Hypothesis& hypothesis,
                                     const ReframingStrategy& reframing) {
    std::ostringstream out;
    out << "# Virtual Lab Hypothesis\n\n";
    out << "- related_case_id: `" << caseId << "`\n";
    out << "- statement: " << hypothesis.statement() << "\n";
    out << "- expected_improvement_pct: " << hypothesis.expectedImprovementPct() << "\n";
    out << "- testable: " << (hypothesis.isTestable() ? "yes" : "no") << "\n";
    out << "- reframing_title: " << reframing.title << "\n";
    out << "- reframing_strategy: " << reframing.entryShift << "\n";
    out << "- expected_perception_shift: " << reframing.expectedPerceptionShift << "\n";
    return out.str();
}

std::string renderExperimentArtifact(const PerceptionShiftReport& report) {
    std::ostringstream out;
    out << "# Virtual Lab Experiment\n\n";
    out << "- case_id: `" << report.caseId << "`\n";
    out << "- latent_rejection: " << report.latentRejection << "\n";
    out << "- activation_trigger: " << report.activationTrigger << "\n";
    out << "- reframing_strategy: " << report.reframingStrategy << "\n";
    out << "- exposure_design: " << report.exposureDesign << "\n";
    out << "- evidence_count: " << report.evidenceCount << "\n";
    out << "- average_confidence: " << report.averageConfidence << "\n";
    out << "- hypothesis_supported: " << (report.hypothesisSupported ? "yes" : "no") << "\n";
    out << "- decision: " << report.decision << "\n";
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

void ensureDirectory(const fs::path& path) {
    std::error_code ec;
    fs::create_directories(path, ec);
    if (ec) {
        throw std::runtime_error("failed to create directory: " + path.string());
    }
}

void ensureFileIfMissing(const fs::path& path, const std::string& content) {
    if (fs::exists(path)) return;
    writeTextFile(path, content);
}

Experiment buildVirtualLabExperiment() {
    LabCase labCase{
        .id = "LV-CASE-101",
        .title = "Introduzir o CRE sem ativar rejeicao por complexidade",
        .context = "Laboratorio Virtual CRE: validar uma entrada por caso concreto antes da teoria completa.",
        .automaticRejection = "Isso parece complexo demais para valer o teste.",
        .trigger = "Excesso de abstracao e linguagem tecnica logo na entrada."
    };
    RejectionSignal rejection{
        .heuristic = "complexidade alta = nao vale testar",
        .observedPhrase = "Isso parece complexo demais para valer o teste."
    };
    ActivationTrigger trigger{
        .primary = "Excesso de abstracao e linguagem tecnica logo na entrada.",
        .secondary = "Apresentacao de arquitetura antes de experiencia concreta."
    };

    Hypothesis hypothesis{
        "Se a abertura iniciar por uma demonstracao concreta de uso, entao a rejeicao inicial reduz porque o valor aparece antes da carga conceitual.",
        15.0
    };
    ReframingStrategy reframing{
        .title = "Valor primeiro, teoria depois",
        .entryShift = "Trocar abertura abstrata por demonstracao curta de uso real.",
        .expectedPerceptionShift = "Mudar de defesa inicial para curiosidade de aplicacao."
    };
    ControlledExposure exposure{
        .design = "Demonstracao curta antes da explicacao arquitetural.",
        .steps = {
            "Apresentar demo de 2 minutos.",
            "Explicar arquitetura apenas apos a demo.",
            "Coletar perguntas dos 10 primeiros minutos."
        },
        .guardrail = "Nao promover reframing sem entrega observavel."
    };

    Experiment experiment{labCase, rejection, trigger, hypothesis, reframing, exposure};
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

Experiment::Experiment(LabCase labCase,
                       RejectionSignal rejection,
                       ActivationTrigger trigger,
                       Hypothesis hypothesis,
                       ReframingStrategy reframing,
                       ControlledExposure exposure)
    : m_labCase(std::move(labCase)),
      m_rejection(std::move(rejection)),
      m_trigger(std::move(trigger)),
      m_hypothesis(std::move(hypothesis)),
      m_reframing(std::move(reframing)),
      m_exposure(std::move(exposure)) {
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

const RejectionSignal& Experiment::rejection() const {
    return m_rejection;
}

const ActivationTrigger& Experiment::activationTrigger() const {
    return m_trigger;
}

const Hypothesis& Experiment::hypothesis() const {
    return m_hypothesis;
}

const ReframingStrategy& Experiment::reframing() const {
    return m_reframing;
}

const ControlledExposure& Experiment::exposure() const {
    return m_exposure;
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

PerceptionShiftReport runVirtualLabCycleDemo() {
    const Experiment experiment = createVirtualLabCycleDemo();

    PerceptionShiftReport report;
    report.caseId = experiment.labCase().id;
    report.latentRejection = experiment.rejection().heuristic;
    report.activationTrigger = experiment.activationTrigger().primary;
    report.reframingStrategy = experiment.reframing().entryShift;
    report.exposureDesign = experiment.exposure().design;
    report.hypothesisSupported = experiment.supportsHypothesis();
    report.decision = report.hypothesisSupported ? "promover" : "iterar";
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

std::string renderExperimentReport(const PerceptionShiftReport& report) {
    std::ostringstream out;
    out << buildLabStatusMessage() << "\n";
    out << "Rejeicao: " << report.latentRejection << "\n";
    out << "Gatilho: " << report.activationTrigger << "\n";
    out << "Reframing: " << report.reframingStrategy << "\n";
    out << "Exposicao: " << report.exposureDesign << "\n";
    out << "Decisao: " << report.decision << "\n";
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
    writeTextFile(artifacts.hypothesisPath,
                  renderHypothesisArtifact(experiment.labCase().id, experiment.hypothesis(), experiment.reframing()));
    writeTextFile(artifacts.experimentPath, renderExperimentArtifact(report));
    writeTextFile(artifacts.evidencePath, renderEvidenceArtifact(experiment.evidences()));

    return artifacts;
}

fs::path findProjectRoot(const fs::path& startDir) {
    std::error_code ec;
    fs::path current = fs::absolute(startDir, ec);
    if (ec) throw std::runtime_error("failed to resolve project root from start directory");

    while (!current.empty()) {
        if (fs::exists(current / "README.md") && fs::exists(current / "ddd" / "LaboratorioVirtual.md")) {
            return current;
        }
        const fs::path parent = current.parent_path();
        if (parent == current) break;
        current = parent;
    }

    throw std::runtime_error("failed to locate CRE project root");
}

WorkspaceLayout resolveWorkspaceLayout(const fs::path& startDir,
                                       const std::optional<fs::path>& explicitWorkspaceRoot,
                                       std::string_view roundId) {
    const fs::path projectRoot = findProjectRoot(startDir);

    fs::path workspaceRoot;
    if (explicitWorkspaceRoot.has_value() && !explicitWorkspaceRoot->empty()) {
        workspaceRoot = fs::absolute(*explicitWorkspaceRoot);
    } else if (const char* envWorkspace = std::getenv("CRE_WORKSPACE");
               envWorkspace != nullptr && std::string_view(envWorkspace).size() > 0) {
        workspaceRoot = fs::absolute(envWorkspace);
    } else if (const auto manifestRoot = tryResolveManifestRoot(startDir)) {
        workspaceRoot = *manifestRoot;
    } else {
        workspaceRoot = projectRoot / "workspace";
    }

    const std::string resolvedRoundId = roundId.empty() ? std::string(kDefaultRoundId) : std::string(roundId);
    return WorkspaceLayout{
        .projectRoot = projectRoot,
        .workspaceRoot = workspaceRoot,
        .manifestPath = workspaceRoot / "cre.yaml",
        .roundsRoot = workspaceRoot / "rounds",
        .activeRoundRoot = workspaceRoot / "rounds" / resolvedRoundId,
        .legacyRoundRoot = projectRoot / "output" / resolvedRoundId
    };
}

void ensureWorkspaceInitialized(const WorkspaceLayout& layout) {
    ensureDirectory(layout.workspaceRoot);
    ensureDirectory(layout.roundsRoot);
    ensureDirectory(layout.workspaceRoot / "cases");
    ensureDirectory(layout.workspaceRoot / "hypotheses");
    ensureDirectory(layout.workspaceRoot / "experiments");
    ensureDirectory(layout.workspaceRoot / "evidences");
    ensureDirectory(layout.workspaceRoot / "cycles");
    ensureDirectory(layout.workspaceRoot / "logs");
    ensureDirectory(layout.workspaceRoot / "exports");
    ensureDirectory(layout.workspaceRoot / ".cre");
    ensureDirectory(layout.workspaceRoot / ".cre" / "cache");
    ensureFileIfMissing(layout.manifestPath, buildWorkspaceManifest());
}

RoundArtifacts recordVirtualLabRoundInWorkspace(const WorkspaceLayout& layout, bool mirrorLegacyOutput) {
    ensureWorkspaceInitialized(layout);
    const RoundArtifacts artifacts = recordVirtualLabRound(layout.activeRoundRoot);
    if (mirrorLegacyOutput) {
        const RoundArtifacts legacyArtifacts = recordVirtualLabRound(layout.legacyRoundRoot);
        (void)legacyArtifacts;
    }
    return artifacts;
}

std::vector<fs::path> candidateRoundDirectories(const WorkspaceLayout& layout) {
    std::vector<fs::path> candidates;
    candidates.push_back(layout.activeRoundRoot);
    if (layout.legacyRoundRoot != layout.activeRoundRoot) {
        candidates.push_back(layout.legacyRoundRoot);
    }
    return candidates;
}

std::vector<fs::path> listWorkspaceRounds(const WorkspaceLayout& layout) {
    std::vector<fs::path> roundDirs;
    std::error_code ec;
    if (fs::exists(layout.roundsRoot, ec) && fs::is_directory(layout.roundsRoot, ec)) {
        for (const auto& entry : fs::directory_iterator(layout.roundsRoot, fs::directory_options::skip_permission_denied, ec)) {
            if (!entry.is_directory(ec)) continue;
            roundDirs.push_back(entry.path());
        }
    }

    if (std::find(roundDirs.begin(), roundDirs.end(), layout.activeRoundRoot) == roundDirs.end()) {
        roundDirs.push_back(layout.activeRoundRoot);
    }

    std::sort(roundDirs.begin(), roundDirs.end(), [&](const fs::path& a, const fs::path& b) {
        std::error_code aec;
        std::error_code bec;
        const auto at = fs::last_write_time(a, aec);
        const auto bt = fs::last_write_time(b, bec);
        if (!aec && !bec && at != bt) return at > bt;
        return a.filename().string() < b.filename().string();
    });
    return roundDirs;
}

} // namespace cre
