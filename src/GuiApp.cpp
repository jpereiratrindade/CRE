#include "cre/GuiApp.hpp"

#include "cre/App.hpp"

#include "backends/imgui_impl_opengl3.h"
#include "backends/imgui_impl_sdl2.h"
#include "imgui.h"

#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>

#include <cstdio>
#include <exception>
#include <filesystem>
#include <fstream>
#include <optional>
#include <sstream>
#include <string>
#include <vector>

namespace cre {
namespace {
namespace fs = std::filesystem;

struct ArtifactEvidenceEntry {
    std::string id;
    std::string source;
    std::string observation;
    std::string confidence;
};

struct ArtifactRoundData {
    bool loaded{false};
    fs::path rootDir;
    std::string caseId;
    std::string caseTitle;
    std::string caseContext;
    std::string automaticRejection;
    std::string trigger;
    std::string hypothesisStatement;
    std::string expectedImprovementPct;
    std::string evidenceCount;
    std::string averageConfidence;
    std::string hypothesisSupported;
    std::string summary;
    std::vector<ArtifactEvidenceEntry> evidences;
    std::string caseMarkdown;
    std::string hypothesisMarkdown;
    std::string experimentMarkdown;
    std::string evidenceMarkdown;
};

struct ProjectDocument {
    std::string label;
    fs::path path;
    std::string description;
    std::string content;
};

std::string trimCopy(const std::string& value) {
    const auto start = value.find_first_not_of(" \t\r\n");
    if (start == std::string::npos) return {};
    const auto end = value.find_last_not_of(" \t\r\n");
    return value.substr(start, end - start + 1);
}

std::string stripMarkdownCode(const std::string& value) {
    if (value.size() >= 2 && value.front() == '`' && value.back() == '`') {
        return value.substr(1, value.size() - 2);
    }
    return value;
}

std::string readTextFile(const fs::path& path) {
    std::ifstream in(path);
    if (!in.is_open()) return {};
    std::ostringstream buffer;
    buffer << in.rdbuf();
    return buffer.str();
}

std::optional<fs::path> findProjectRootForDocs(const fs::path& startDir) {
    fs::path current = fs::absolute(startDir);
    while (!current.empty()) {
        const fs::path readmePath = current / "README.md";
        const fs::path lvPath = current / "ddd" / "LaboratorioVirtual.md";
        if (fs::exists(readmePath) && fs::exists(lvPath)) return current;
        const fs::path parent = current.parent_path();
        if (parent == current) break;
        current = parent;
    }
    return std::nullopt;
}

std::vector<ProjectDocument> loadProjectDocuments(const fs::path& projectRoot) {
    std::vector<ProjectDocument> documents{
        {
            "README",
            projectRoot / "README.md",
            "Visao geral do projeto, bootstrap, CLI e fundamentos do CRE.",
            {}
        },
        {
            "LV",
            projectRoot / "ddd" / "LaboratorioVirtual.md",
            "Base oficial do Laboratorio Virtual: entidades, fluxo, estados e invariantes.",
            {}
        },
        {
            "CRE Core",
            projectRoot / "ddd" / "ContextReframingEngine.md",
            "Nucleo conceitual do Context Reframing Engine e seus limites de dominio.",
            {}
        },
        {
            "ADR Fundador",
            projectRoot / "adr" / "ADR-AGENT-005-cre-foundation-virtual-lab.md",
            "Decisao arquitetural que define CRE + Laboratorio Virtual.",
            {}
        },
        {
            "Etica",
            projectRoot / "policies" / "ethics_limits.md",
            "Limites eticos e guardrails para o uso do CRE.",
            {}
        }
    };

    for (auto& document : documents) {
        document.content = readTextFile(document.path);
        if (document.content.empty()) {
            document.content = "Documento nao encontrado em:\n" + document.path.string();
        }
    }
    return documents;
}

std::string extractBulletValue(const std::string& markdown, const std::string& key) {
    std::istringstream lines(markdown);
    const std::string needle = "- " + key + ":";
    std::string line;
    while (std::getline(lines, line)) {
        if (line.rfind(needle, 0) != 0) continue;
        return stripMarkdownCode(trimCopy(line.substr(needle.size())));
    }
    return {};
}

std::vector<ArtifactEvidenceEntry> parseEvidenceEntries(const std::string& markdown) {
    std::vector<ArtifactEvidenceEntry> entries;
    std::istringstream lines(markdown);
    std::string line;
    ArtifactEvidenceEntry current;
    bool inEntry = false;
    while (std::getline(lines, line)) {
        const std::string trimmed = trimCopy(line);
        if (trimmed.rfind("## ", 0) == 0) {
            if (inEntry) entries.push_back(current);
            current = {};
            current.id = trimCopy(trimmed.substr(3));
            inEntry = true;
            continue;
        }
        if (!inEntry) continue;
        if (trimmed.rfind("- source:", 0) == 0) {
            current.source = trimCopy(trimmed.substr(std::string("- source:").size()));
        } else if (trimmed.rfind("- observation:", 0) == 0) {
            current.observation = trimCopy(trimmed.substr(std::string("- observation:").size()));
        } else if (trimmed.rfind("- confidence:", 0) == 0) {
            current.confidence = trimCopy(trimmed.substr(std::string("- confidence:").size()));
        }
    }
    if (inEntry) entries.push_back(current);
    return entries;
}

std::optional<ArtifactRoundData> loadArtifactRoundData(const fs::path& rootDir) {
    const fs::path casePath = rootDir / "LV-CASE-101.md";
    const fs::path hypothesisPath = rootDir / "LV-HYP-101.md";
    const fs::path experimentPath = rootDir / "LV-EXP-101.md";
    const fs::path evidencePath = rootDir / "LV-EVD-LOG-101.md";

    if (!fs::exists(casePath) || !fs::exists(hypothesisPath) ||
        !fs::exists(experimentPath) || !fs::exists(evidencePath)) {
        return std::nullopt;
    }

    ArtifactRoundData data;
    data.loaded = true;
    data.rootDir = rootDir;
    data.caseMarkdown = readTextFile(casePath);
    data.hypothesisMarkdown = readTextFile(hypothesisPath);
    data.experimentMarkdown = readTextFile(experimentPath);
    data.evidenceMarkdown = readTextFile(evidencePath);

    data.caseId = extractBulletValue(data.caseMarkdown, "case_id");
    data.caseTitle = extractBulletValue(data.caseMarkdown, "title");
    data.caseContext = extractBulletValue(data.caseMarkdown, "context");
    data.automaticRejection = extractBulletValue(data.caseMarkdown, "automatic_rejection");
    data.trigger = extractBulletValue(data.caseMarkdown, "trigger");
    data.hypothesisStatement = extractBulletValue(data.hypothesisMarkdown, "statement");
    data.expectedImprovementPct = extractBulletValue(data.hypothesisMarkdown, "expected_improvement_pct");
    data.evidenceCount = extractBulletValue(data.experimentMarkdown, "evidence_count");
    data.averageConfidence = extractBulletValue(data.experimentMarkdown, "average_confidence");
    data.hypothesisSupported = extractBulletValue(data.experimentMarkdown, "hypothesis_supported");
    data.summary = extractBulletValue(data.experimentMarkdown, "summary");
    data.evidences = parseEvidenceEntries(data.evidenceMarkdown);
    return data;
}

std::optional<ArtifactRoundData> loadArtifactRoundData(const std::vector<fs::path>& candidates) {
    for (const auto& candidate : candidates) {
        if (auto data = loadArtifactRoundData(candidate)) return data;
    }
    return std::nullopt;
}

void renderTopBar(const LabCase& labCase, const Hypothesis& hypothesis, const Experiment& experiment) {
    ImGui::Text("Rodada ativa | Caso: %s | Evidencias: %d", labCase.id.c_str(),
                static_cast<int>(experiment.evidences().size()));
    ImGui::TextDisabled("Hipotese: %s", hypothesis.statement().c_str());
    ImGui::Separator();
}

void renderTopBar(const ArtifactRoundData& data) {
    ImGui::Text("Rodada carregada | Caso: %s | Evidencias: %d", data.caseId.c_str(),
                static_cast<int>(data.evidences.size()));
    ImGui::TextDisabled("Hipotese: %s", data.hypothesisStatement.c_str());
    ImGui::TextDisabled("Fonte: %s", data.rootDir.string().c_str());
    ImGui::Separator();
}

void renderRodadaTab(const Experiment& experiment, const VirtualLabReport& report) {
    ImGui::TextWrapped("%s", buildLabStatusMessage().c_str());
    ImGui::Spacing();
    ImGui::BulletText("Caso ativo: %s", experiment.labCase().title.c_str());
    ImGui::BulletText("Hipotese ativa: %s", experiment.hypothesis().statement().c_str());
    ImGui::BulletText("Evidencias registradas: %d", static_cast<int>(report.evidenceCount));
    ImGui::BulletText("Confianca media: %.2f", report.averageConfidence);
    ImGui::BulletText("Decisao sugerida: %s", report.hypothesisSupported ? "promover" : "iterar");
    ImGui::Spacing();
    ImGui::TextWrapped("%s", report.summary.c_str());
}

void renderCasosTab(const Experiment& experiment) {
    const LabCase& labCase = experiment.labCase();
    ImGui::Text("Caso de referencia");
    ImGui::Separator();
    ImGui::Text("ID: %s", labCase.id.c_str());
    ImGui::Text("Titulo: %s", labCase.title.c_str());
    ImGui::TextWrapped("Contexto: %s", labCase.context.c_str());
    ImGui::TextWrapped("Rejeicao automatica: %s", labCase.automaticRejection.c_str());
    ImGui::TextWrapped("Gatilho: %s", labCase.trigger.c_str());
}

void renderHipotesesTab(const Experiment& experiment) {
    const Hypothesis& hypothesis = experiment.hypothesis();
    ImGui::Text("Hipotese ativa");
    ImGui::Separator();
    ImGui::TextWrapped("%s", hypothesis.statement().c_str());
    ImGui::BulletText("Melhoria esperada: %.2f%%", hypothesis.expectedImprovementPct());
    ImGui::BulletText("Testavel: %s", hypothesis.isTestable() ? "sim" : "nao");
}

void renderExperimentosTab(const Experiment& experiment) {
    ImGui::Text("Experimento atual");
    ImGui::Separator();
    ImGui::BulletText("Estado: %s", experiment.wasExecuted() ? "executado" : "nao executado");
    ImGui::BulletText("Guardrail principal: nao promover reframing sem entrega real");
    ImGui::BulletText("Criterio de corroboracao: confianca media >= 0.70");
}

void renderEvidenciasTab(const Experiment& experiment) {
    ImGui::Text("Evidencias");
    ImGui::Separator();
    for (const auto& evidence : experiment.evidences()) {
        ImGui::PushID(evidence.id.c_str());
        ImGui::Text("%s", evidence.id.c_str());
        ImGui::TextDisabled("Fonte: %s | Confianca: %.2f", evidence.source.c_str(), evidence.confidence);
        ImGui::TextWrapped("%s", evidence.observation.c_str());
        ImGui::Separator();
        ImGui::PopID();
    }
}

void renderDecisaoTab(const VirtualLabReport& report) {
    ImGui::Text("Decisao da rodada");
    ImGui::Separator();
    ImGui::BulletText("Hipotese corroborada: %s", report.hypothesisSupported ? "sim" : "nao");
    ImGui::BulletText("Acao recomendada: %s", report.hypothesisSupported ? "promover" : "iterar");
    ImGui::BulletText("Risco residual: validar em nova rodada com mais evidencias");
}

void renderRodadaTab(const ArtifactRoundData& data) {
    ImGui::TextWrapped("%s", buildLabStatusMessage().c_str());
    ImGui::Spacing();
    ImGui::BulletText("Caso ativo: %s", data.caseTitle.c_str());
    ImGui::BulletText("Hipotese ativa: %s", data.hypothesisStatement.c_str());
    ImGui::BulletText("Evidencias registradas: %s", data.evidenceCount.c_str());
    ImGui::BulletText("Confianca media: %s", data.averageConfidence.c_str());
    ImGui::BulletText("Hipotese corroborada: %s", data.hypothesisSupported.c_str());
    ImGui::Spacing();
    ImGui::TextWrapped("%s", data.summary.c_str());
}

void renderCasosTab(const ArtifactRoundData& data) {
    ImGui::Text("Caso carregado de artefato");
    ImGui::Separator();
    ImGui::Text("ID: %s", data.caseId.c_str());
    ImGui::Text("Titulo: %s", data.caseTitle.c_str());
    ImGui::TextWrapped("Contexto: %s", data.caseContext.c_str());
    ImGui::TextWrapped("Rejeicao automatica: %s", data.automaticRejection.c_str());
    ImGui::TextWrapped("Gatilho: %s", data.trigger.c_str());
    ImGui::Separator();
    ImGui::TextUnformatted(data.caseMarkdown.c_str());
}

void renderHipotesesTab(const ArtifactRoundData& data) {
    ImGui::Text("Hipotese carregada");
    ImGui::Separator();
    ImGui::TextWrapped("%s", data.hypothesisStatement.c_str());
    ImGui::BulletText("Melhoria esperada: %s%%", data.expectedImprovementPct.c_str());
    ImGui::Separator();
    ImGui::TextUnformatted(data.hypothesisMarkdown.c_str());
}

void renderExperimentosTab(const ArtifactRoundData& data) {
    ImGui::Text("Experimento carregado");
    ImGui::Separator();
    ImGui::BulletText("Evidencias: %s", data.evidenceCount.c_str());
    ImGui::BulletText("Confianca media: %s", data.averageConfidence.c_str());
    ImGui::BulletText("Hipotese corroborada: %s", data.hypothesisSupported.c_str());
    ImGui::Separator();
    ImGui::TextUnformatted(data.experimentMarkdown.c_str());
}

void renderEvidenciasTab(const ArtifactRoundData& data) {
    ImGui::Text("Evidencias carregadas");
    ImGui::Separator();
    for (const auto& evidence : data.evidences) {
        ImGui::PushID(evidence.id.c_str());
        ImGui::Text("%s", evidence.id.c_str());
        ImGui::TextDisabled("Fonte: %s | Confianca: %s", evidence.source.c_str(), evidence.confidence.c_str());
        ImGui::TextWrapped("%s", evidence.observation.c_str());
        ImGui::Separator();
        ImGui::PopID();
    }
}

void renderDecisaoTab(const ArtifactRoundData& data) {
    ImGui::Text("Decisao derivada dos artefatos");
    ImGui::Separator();
    ImGui::BulletText("Hipotese corroborada: %s", data.hypothesisSupported.c_str());
    ImGui::BulletText("Resumo: %s", data.summary.c_str());
    ImGui::BulletText("Proxima acao sugerida: %s",
                      data.hypothesisSupported == "yes" ? "promover" : "iterar");
}

void renderDocumentsTab(const std::vector<ProjectDocument>& documents, int& selectedDocument) {
    ImGui::Text("Ajuda e acesso rapido aos documentos do CRE");
    ImGui::Separator();
    ImGui::TextDisabled("Atalhos: F1 abre esta aba | Ctrl+1 README | Ctrl+2 LV | Ctrl+3 Core | Ctrl+4 ADR | Ctrl+5 Etica");
    ImGui::Spacing();

    ImGui::BeginChild("docs_list", ImVec2(240.f, 0.f), true);
    for (int i = 0; i < static_cast<int>(documents.size()); ++i) {
        const bool isSelected = selectedDocument == i;
        if (ImGui::Selectable(documents[i].label.c_str(), isSelected)) {
            selectedDocument = i;
        }
    }
    ImGui::EndChild();

    ImGui::SameLine();

    ImGui::BeginChild("docs_view", ImVec2(0.f, 0.f), true);
    if (selectedDocument >= 0 && selectedDocument < static_cast<int>(documents.size())) {
        const ProjectDocument& document = documents[selectedDocument];
        ImGui::Text("%s", document.label.c_str());
        ImGui::TextDisabled("%s", document.path.string().c_str());
        if (ImGui::Button("Copiar caminho")) {
            ImGui::SetClipboardText(document.path.string().c_str());
        }
        ImGui::Separator();
        ImGui::TextWrapped("%s", document.description.c_str());
        ImGui::Spacing();
        ImGui::BeginChild("doc_content", ImVec2(0.f, 0.f), false,
                          ImGuiWindowFlags_HorizontalScrollbar);
        ImGui::TextUnformatted(document.content.c_str());
        ImGui::EndChild();
    } else {
        ImGui::TextDisabled("Nenhum documento selecionado.");
    }
    ImGui::EndChild();
}

} // namespace

int runGuiApp() {
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER) != 0) {
        std::fprintf(stderr, "SDL_Init error: %s\n", SDL_GetError());
        return 1;
    }

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, 0);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
    SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);

    SDL_Window* window = SDL_CreateWindow(
        "CRE - Laboratorio Virtual",
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        1280, 800,
        SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE
    );
    if (!window) {
        std::fprintf(stderr, "SDL_CreateWindow error: %s\n", SDL_GetError());
        SDL_Quit();
        return 1;
    }

    SDL_GLContext glContext = SDL_GL_CreateContext(window);
    SDL_GL_MakeCurrent(window, glContext);
    SDL_GL_SetSwapInterval(1);

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    (void)io;
    ImGui::StyleColorsDark();

    ImGui_ImplSDL2_InitForOpenGL(window, glContext);
    ImGui_ImplOpenGL3_Init("#version 330");

    const Experiment experiment = createVirtualLabCycleDemo();
    const VirtualLabReport report = runVirtualLabCycleDemo();
    const WorkspaceLayout workspace = resolveWorkspaceLayout(fs::current_path());
    ensureWorkspaceInitialized(workspace);
    const fs::path projectRoot = findProjectRootForDocs(workspace.projectRoot).value_or(workspace.projectRoot);
    std::vector<ProjectDocument> documents = loadProjectDocuments(projectRoot);
    std::optional<ArtifactRoundData> artifactData = loadArtifactRoundData(candidateRoundDirectories(workspace));
    int selectedDocument = 0;
    bool focusDocumentsTab = false;

    bool running = true;
    while (running) {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            ImGui_ImplSDL2_ProcessEvent(&event);
            if (event.type == SDL_QUIT) running = false;
            if (event.type == SDL_WINDOWEVENT &&
                event.window.event == SDL_WINDOWEVENT_CLOSE &&
                event.window.windowID == SDL_GetWindowID(window)) {
                running = false;
            }
            if (event.type == SDL_KEYDOWN) {
                const bool ctrlPressed = (event.key.keysym.mod & KMOD_CTRL) != 0;
                if (event.key.keysym.sym == SDLK_F1) {
                    focusDocumentsTab = true;
                } else if (ctrlPressed && event.key.keysym.sym >= SDLK_1 &&
                           event.key.keysym.sym <= SDLK_5) {
                    selectedDocument = static_cast<int>(event.key.keysym.sym - SDLK_1);
                    focusDocumentsTab = true;
                }
            }
        }

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplSDL2_NewFrame();
        ImGui::NewFrame();

        ImGui::SetNextWindowPos(ImVec2(0.f, 0.f), ImGuiCond_Always);
        ImGui::SetNextWindowSize(io.DisplaySize, ImGuiCond_Always);

        ImGuiWindowFlags flags = ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove |
                                 ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoBringToFrontOnFocus;
        ImGui::Begin("CRE Shell", nullptr, flags);

        if (artifactData.has_value()) {
            renderTopBar(*artifactData);
        } else {
            renderTopBar(experiment.labCase(), experiment.hypothesis(), experiment);
        }

        if (ImGui::Button("Ajuda Docs (F1)")) {
            focusDocumentsTab = true;
        }
        ImGui::SameLine();
        if (ImGui::Button("Recarregar Docs")) {
            documents = loadProjectDocuments(projectRoot);
        }
        ImGui::SameLine();
        ImGui::TextDisabled("Workspace: %s", workspace.workspaceRoot.string().c_str());
        ImGui::Separator();

        if (ImGui::BeginTabBar("LVTabs")) {
            if (ImGui::BeginTabItem("Rodada")) {
                if (artifactData.has_value()) renderRodadaTab(*artifactData);
                else renderRodadaTab(experiment, report);
                ImGui::EndTabItem();
            }
            if (ImGui::BeginTabItem("Casos")) {
                if (artifactData.has_value()) renderCasosTab(*artifactData);
                else renderCasosTab(experiment);
                ImGui::EndTabItem();
            }
            if (ImGui::BeginTabItem("Hipoteses")) {
                if (artifactData.has_value()) renderHipotesesTab(*artifactData);
                else renderHipotesesTab(experiment);
                ImGui::EndTabItem();
            }
            if (ImGui::BeginTabItem("Experimentos")) {
                if (artifactData.has_value()) renderExperimentosTab(*artifactData);
                else renderExperimentosTab(experiment);
                ImGui::EndTabItem();
            }
            if (ImGui::BeginTabItem("Evidencias")) {
                if (artifactData.has_value()) renderEvidenciasTab(*artifactData);
                else renderEvidenciasTab(experiment);
                ImGui::EndTabItem();
            }
            if (ImGui::BeginTabItem("Decisao")) {
                if (artifactData.has_value()) renderDecisaoTab(*artifactData);
                else renderDecisaoTab(report);
                ImGui::EndTabItem();
            }
            ImGuiTabItemFlags docsFlags = focusDocumentsTab ? ImGuiTabItemFlags_SetSelected : 0;
            if (ImGui::BeginTabItem("Documentos", nullptr, docsFlags)) {
                focusDocumentsTab = false;
                renderDocumentsTab(documents, selectedDocument);
                ImGui::EndTabItem();
            }
            ImGui::EndTabBar();
        }

        ImGui::Separator();
        if (ImGui::Button("Registrar Rodada Demo no Workspace")) {
            try {
                const RoundArtifacts artifacts = recordVirtualLabRoundInWorkspace(workspace);
                (void)artifacts;
                artifactData = loadArtifactRoundData(candidateRoundDirectories(workspace));
            } catch (const std::exception& ex) {
                std::fprintf(stderr, "recordVirtualLabRound failed: %s\n", ex.what());
            }
        }
        ImGui::SameLine();
        if (ImGui::Button("Recarregar Artefatos")) {
            artifactData = loadArtifactRoundData(candidateRoundDirectories(workspace));
        }
        ImGui::SameLine();
        ImGui::TextDisabled("%s",
                            artifactData.has_value() ? workspace.activeRoundRoot.string().c_str()
                                                     : "Sem artefatos locais: exibindo rodada demo em memoria");

        ImGui::End();

        ImGui::Render();
        int displayW = 0;
        int displayH = 0;
        SDL_GL_GetDrawableSize(window, &displayW, &displayH);
        glViewport(0, 0, displayW, displayH);
        glClearColor(0.08f, 0.09f, 0.12f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        SDL_GL_SwapWindow(window);
    }

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplSDL2_Shutdown();
    ImGui::DestroyContext();
    SDL_GL_DeleteContext(glContext);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}

} // namespace cre
