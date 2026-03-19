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
constexpr std::string_view kDefaultScenarioId = "cre-intro";

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

std::string zeroPadNumber(int value, int width = 3) {
    std::ostringstream out;
    out.width(width);
    out.fill('0');
    out << value;
    return out.str();
}

ScenarioDescriptor findScenarioDescriptor(std::string_view scenarioId) {
    for (const auto& scenario : listVirtualLabScenarios()) {
        if (scenario.id == scenarioId) return scenario;
    }
    return {
        std::string(scenarioId.empty() ? kDefaultScenarioId : scenarioId),
        std::string(scenarioId.empty() ? kDefaultScenarioId : scenarioId),
        "Cenario sem descriptor explicito."
    };
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

std::string renderCaseArtifact(const LabCase& labCase, std::string_view observedPhrase) {
    std::ostringstream out;
    out << renderCaseArtifact(labCase);
    if (!observedPhrase.empty()) {
        out << "- observed_phrase: " << observedPhrase << "\n";
    }
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

std::string renderRoundMetadataArtifact(const ScenarioDescriptor& scenario,
                                        const fs::path& outputDir,
                                        const LabCase& labCase,
                                        std::string_view ethicalLimit = {}) {
    std::ostringstream out;
    out << "# Virtual Lab Round Metadata\n\n";
    out << "- scenario_id: " << scenario.id << "\n";
    out << "- scenario_label: " << scenario.label << "\n";
    out << "- scenario_description: " << scenario.description << "\n";
    out << "- round_id: " << outputDir.filename().string() << "\n";
    out << "- case_id: `" << labCase.id << "`\n";
    if (!ethicalLimit.empty()) out << "- ethical_limit: " << ethicalLimit << "\n";
    out << "- root_dir: " << outputDir.string() << "\n";
    return out.str();
}

std::string artifactIdFromCaseId(const std::string& caseId, std::string_view targetToken) {
    std::string id = caseId;
    const std::string caseToken = "CASE";
    const std::size_t pos = id.find(caseToken);
    if (pos != std::string::npos) {
        id.replace(pos, caseToken.size(), targetToken);
        return id;
    }
    return std::string(targetToken) + "-" + caseId;
}

int extractTrailingNumber(const std::string& value) {
    std::size_t pos = value.find_last_not_of("0123456789");
    if (pos == std::string::npos) return std::stoi(value);
    if (pos + 1 >= value.size()) return 0;
    return std::stoi(value.substr(pos + 1));
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

Experiment buildVirtualLabExperiment(std::string_view scenarioId) {
    if (scenarioId == "education-inclusion") {
        LabCase labCase{
            .id = "LV-CASE-201",
            .title = "Apresentar inclusao como melhoria de experiencia, nao como obrigacao",
            .context = "Contexto educacional com resistencia inicial a temas percebidos como normativos ou pesados.",
            .automaticRejection = "Isso vai ser mais uma palestra obrigatoria e cansativa.",
            .trigger = "Uso imediato do termo inclusao em tom institucional e normativo."
        };
        RejectionSignal rejection{
            .heuristic = "inclusao = obrigacao chata",
            .observedPhrase = "Isso vai ser mais uma palestra obrigatoria e cansativa."
        };
        ActivationTrigger trigger{
            .primary = "Uso imediato do termo inclusao em tom institucional e normativo.",
            .secondary = "Abertura sem caso concreto de friccao vivida no cotidiano."
        };
        Hypothesis hypothesis{
            "Se o tema entrar como melhoria de experiencia compartilhada e resolucao de friccoes concretas, entao o engajamento aumenta sem resistencia inicial.",
            22.0
        };
        ReframingStrategy reframing{
            .title = "Experiencia primeiro, rotulo depois",
            .entryShift = "Comecar por situacoes reais de atrito e colaboracao antes de nomear o tema como inclusao.",
            .expectedPerceptionShift = "Trocar defesa normativa por participacao pratica."
        };
        ControlledExposure exposure{
            .design = "Abrir com caso vivido em sala e coleta de solucoes antes da nomeacao do tema.",
            .steps = {
                "Apresentar um caso curto de friccao em ambiente de aprendizagem.",
                "Pedir solucoes praticas ao grupo.",
                "Conectar as solucoes ao conceito de inclusao so depois."
            },
            .guardrail = "Nao mascarar o tema; apenas mudar o ponto de entrada."
        };

        Experiment experiment{labCase, rejection, trigger, hypothesis, reframing, exposure};
        experiment.addEvidence({
            .id = "LV-EVD-201",
            .source = "piloto-sala-aula-v1",
            .observation = "Os participantes propuseram solucoes concretas antes de questionar o tema.",
            .confidence = 0.81
        });
        experiment.addEvidence({
            .id = "LV-EVD-202",
            .source = "feedback-qualitativo-v1",
            .observation = "A percepcao mudou de obrigacao institucional para melhoria coletiva de experiencia.",
            .confidence = 0.84
        });
        experiment.execute();
        return experiment;
    }

    if (scenarioId == "strata-entry") {
        LabCase labCase{
            .id = "LV-CASE-301",
            .title = "Introduzir o STRATA por caso visivel antes da arquitetura completa",
            .context = "Projeto complexo com risco de rejeicao quando a apresentacao comeca pela estrutura total do sistema.",
            .automaticRejection = "Isso e complexo demais para eu entender agora.",
            .trigger = "Apresentacao do sistema completo antes de um caso simples de uso."
        };
        RejectionSignal rejection{
            .heuristic = "complexidade aparente = melhor nao entrar",
            .observedPhrase = "Isso e complexo demais para eu entender agora."
        };
        ActivationTrigger trigger{
            .primary = "Apresentacao do sistema completo antes de um caso simples de uso.",
            .secondary = "Excesso de linguagem estrutural na primeira exposicao."
        };
        Hypothesis hypothesis{
            "Se o STRATA entrar por um caso visual concreto de leitura de situacao e so depois mostrar a arquitetura, entao a curiosidade cresce e a resistencia cai.",
            18.0
        };
        ReframingStrategy reframing{
            .title = "Caso visual antes do mapa completo",
            .entryShift = "Entrar pelo que o sistema ajuda a enxergar antes de explicar todos os modulos.",
            .expectedPerceptionShift = "Trocar intimidacao arquitetural por curiosidade orientada a uso."
        };
        ControlledExposure exposure{
            .design = "Visualizacao de um caso simples seguida de decomposicao progressiva do sistema.",
            .steps = {
                "Mostrar uma leitura visual simples produzida pelo sistema.",
                "Perguntar o que a pessoa conseguiu perceber.",
                "Apresentar a arquitetura em camadas depois do caso."
            },
            .guardrail = "Nao prometer simplicidade falsa; a progressao precisa continuar honesta."
        };

        Experiment experiment{labCase, rejection, trigger, hypothesis, reframing, exposure};
        experiment.addEvidence({
            .id = "LV-EVD-301",
            .source = "piloto-strata-v1",
            .observation = "A conversa permaneceu no valor percebido antes de migrar para a arquitetura.",
            .confidence = 0.78
        });
        experiment.addEvidence({
            .id = "LV-EVD-302",
            .source = "observacao-sessao-v1",
            .observation = "A arquitetura passou a ser recebida como explicacao de algo util, nao como barreira inicial.",
            .confidence = 0.8
        });
        experiment.execute();
        return experiment;
    }

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

std::vector<ScenarioDescriptor> listVirtualLabScenarios() {
    return {
        {
            "cre-intro",
            "CRE Intro",
            "Entrada do CRE por demonstracao concreta antes da teoria completa."
        },
        {
            "education-inclusion",
            "Educacao Inclusiva",
            "Reframing de inclusao como melhoria de experiencia e nao obrigacao normativa."
        },
        {
            "strata-entry",
            "STRATA Entry",
            "Entrada do STRATA por caso visual antes da arquitetura completa."
        }
    };
}

std::string buildNextRoundId(const WorkspaceLayout& layout) {
    int maxValue = 0;
    for (const auto& roundDir : listWorkspaceRounds(layout)) {
        const std::string name = roundDir.filename().string();
        if (name.rfind("lv-round-", 0) != 0) continue;
        try {
            maxValue = std::max(maxValue, extractTrailingNumber(name));
        } catch (const std::exception&) {
        }
    }
    return "lv-round-" + zeroPadNumber(maxValue + 1);
}

std::string buildNextCaseId(const WorkspaceLayout& layout) {
    int maxValue = 0;
    std::error_code ec;
    if (fs::exists(layout.roundsRoot, ec) && fs::is_directory(layout.roundsRoot, ec)) {
        for (const auto& entry : fs::recursive_directory_iterator(layout.roundsRoot, fs::directory_options::skip_permission_denied, ec)) {
            if (!entry.is_regular_file(ec)) continue;
            const std::string name = entry.path().filename().string();
            if (name.rfind("LV-CASE-", 0) != 0 || entry.path().extension() != ".md") continue;
            try {
                maxValue = std::max(maxValue, extractTrailingNumber(name));
            } catch (const std::exception&) {
            }
        }
    }
    return "LV-CASE-" + zeroPadNumber(maxValue + 1);
}

Experiment createVirtualLabCycleDemo(std::string_view scenarioId) {
    return buildVirtualLabExperiment(scenarioId.empty() ? kDefaultScenarioId : scenarioId);
}

PerceptionShiftReport runVirtualLabCycleDemo(std::string_view scenarioId) {
    const Experiment experiment = createVirtualLabCycleDemo(scenarioId);

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

RoundArtifacts recordVirtualLabRound(const fs::path& outputDir, std::string_view scenarioId) {
    const Experiment experiment = createVirtualLabCycleDemo(scenarioId);
    const VirtualLabReport report = runVirtualLabCycleDemo(scenarioId);
    const ScenarioDescriptor scenario = findScenarioDescriptor(scenarioId);

    std::error_code ec;
    fs::create_directories(outputDir, ec);
    if (ec) {
        throw std::runtime_error("failed to create output directory: " + outputDir.string());
    }

    RoundArtifacts artifacts{
        .casePath = outputDir / (experiment.labCase().id + ".md"),
        .hypothesisPath = outputDir / (artifactIdFromCaseId(experiment.labCase().id, "HYP") + ".md"),
        .experimentPath = outputDir / (artifactIdFromCaseId(experiment.labCase().id, "EXP") + ".md"),
        .evidencePath = outputDir / (artifactIdFromCaseId(experiment.labCase().id, "EVD-LOG") + ".md"),
        .metadataPath = outputDir / "LV-ROUND-META.md"
    };

    writeTextFile(artifacts.casePath, renderCaseArtifact(experiment.labCase()));
    writeTextFile(artifacts.hypothesisPath,
                  renderHypothesisArtifact(experiment.labCase().id, experiment.hypothesis(), experiment.reframing()));
    writeTextFile(artifacts.experimentPath, renderExperimentArtifact(report));
    writeTextFile(artifacts.evidencePath, renderEvidenceArtifact(experiment.evidences()));
    writeTextFile(artifacts.metadataPath, renderRoundMetadataArtifact(scenario, outputDir, experiment.labCase()));

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

RoundArtifacts recordVirtualLabRoundInWorkspace(const WorkspaceLayout& layout,
                                                bool mirrorLegacyOutput,
                                                std::string_view scenarioId) {
    ensureWorkspaceInitialized(layout);
    const RoundArtifacts artifacts = recordVirtualLabRound(layout.activeRoundRoot, scenarioId);
    if (mirrorLegacyOutput) {
        const RoundArtifacts legacyArtifacts = recordVirtualLabRound(layout.legacyRoundRoot, scenarioId);
        (void)legacyArtifacts;
    }
    return artifacts;
}

RoundArtifacts recordDraftRoundInWorkspace(const WorkspaceLayout& layout, const ReframingDraft& draft) {
    ensureWorkspaceInitialized(layout);

    LabCase labCase{
        .id = draft.caseId,
        .title = draft.caseTitle,
        .context = draft.caseContext,
        .automaticRejection = draft.automaticRejection,
        .trigger = draft.activationTriggerPrimary
    };
    Hypothesis hypothesis{draft.hypothesisStatement, draft.expectedImprovementPct};
    ReframingStrategy reframing{
        .title = draft.reframingTitle,
        .entryShift = draft.reframingStrategy,
        .expectedPerceptionShift = draft.expectedPerceptionShift
    };

    PerceptionShiftReport report;
    report.caseId = draft.caseId;
    report.latentRejection = draft.latentRejection;
    report.activationTrigger = draft.activationTriggerPrimary;
    report.reframingStrategy = draft.reframingStrategy;
    report.exposureDesign = draft.exposureDesign;
    report.hypothesisSupported = false;
    report.decision = "draft";
    report.evidenceCount = 0;
    report.averageConfidence = 0.0;
    report.summary = "Rodada criada manualmente para desenvolvimento do reframing.";

    const ScenarioDescriptor scenario{
        draft.scenarioId,
        draft.scenarioLabel,
        draft.scenarioDescription
    };

    const RoundArtifacts artifacts{
        .casePath = layout.activeRoundRoot / (draft.caseId + ".md"),
        .hypothesisPath = layout.activeRoundRoot / (artifactIdFromCaseId(draft.caseId, "HYP") + ".md"),
        .experimentPath = layout.activeRoundRoot / (artifactIdFromCaseId(draft.caseId, "EXP") + ".md"),
        .evidencePath = layout.activeRoundRoot / (artifactIdFromCaseId(draft.caseId, "EVD-LOG") + ".md"),
        .metadataPath = layout.activeRoundRoot / "LV-ROUND-META.md"
    };

    ensureDirectory(layout.activeRoundRoot);

    writeTextFile(artifacts.casePath, renderCaseArtifact(labCase, draft.observedPhrase));
    writeTextFile(artifacts.hypothesisPath, renderHypothesisArtifact(draft.caseId, hypothesis, reframing));

    std::ostringstream experimentOut;
    experimentOut << "# Virtual Lab Experiment\n\n";
    experimentOut << "- case_id: `" << draft.caseId << "`\n";
    experimentOut << "- latent_rejection: " << draft.latentRejection << "\n";
    experimentOut << "- activation_trigger: " << draft.activationTriggerPrimary << "\n";
    if (!draft.activationTriggerSecondary.empty()) {
        experimentOut << "- activation_trigger_secondary: " << draft.activationTriggerSecondary << "\n";
    }
    experimentOut << "- reframing_strategy: " << draft.reframingStrategy << "\n";
    experimentOut << "- exposure_design: " << draft.exposureDesign << "\n";
    experimentOut << "- exposure_guardrail: " << draft.exposureGuardrail << "\n";
    experimentOut << "- ethical_limit: " << draft.ethicalLimit << "\n";
    experimentOut << "- evidence_count: 0\n";
    experimentOut << "- average_confidence: 0\n";
    experimentOut << "- hypothesis_supported: no\n";
    experimentOut << "- decision: draft\n";
    experimentOut << "- summary: Rodada criada manualmente e aguardando evidencias.\n";
    writeTextFile(artifacts.experimentPath, experimentOut.str());

    std::ostringstream evidenceOut;
    evidenceOut << "# Virtual Lab Evidence Log\n\n";
    evidenceOut << "_Sem evidencias registradas nesta rodada._\n";
    writeTextFile(artifacts.evidencePath, evidenceOut.str());

    writeTextFile(
        artifacts.metadataPath,
        renderRoundMetadataArtifact(scenario, layout.activeRoundRoot, labCase, draft.ethicalLimit));

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
