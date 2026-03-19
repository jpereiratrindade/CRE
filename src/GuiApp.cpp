#include "cre/GuiApp.hpp"

#include "cre/App.hpp"

#include "backends/imgui_impl_opengl3.h"
#include "backends/imgui_impl_sdl2.h"
#include "imgui.h"

#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>

#include <cstdio>
#include <exception>
#include <string>

namespace cre {
namespace {

void renderTopBar(const LabCase& labCase, const Hypothesis& hypothesis, const Experiment& experiment) {
    ImGui::Text("Rodada ativa | Caso: %s | Evidencias: %d", labCase.id.c_str(),
                static_cast<int>(experiment.evidences().size()));
    ImGui::TextDisabled("Hipotese: %s", hypothesis.statement().c_str());
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
        }

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplSDL2_NewFrame();
        ImGui::NewFrame();

        ImGui::SetNextWindowPos(ImVec2(0.f, 0.f), ImGuiCond_Always);
        ImGui::SetNextWindowSize(io.DisplaySize, ImGuiCond_Always);

        ImGuiWindowFlags flags = ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove |
                                 ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoBringToFrontOnFocus;
        ImGui::Begin("CRE Shell", nullptr, flags);

        renderTopBar(experiment.labCase(), experiment.hypothesis(), experiment);

        if (ImGui::BeginTabBar("LVTabs")) {
            if (ImGui::BeginTabItem("Rodada")) {
                renderRodadaTab(experiment, report);
                ImGui::EndTabItem();
            }
            if (ImGui::BeginTabItem("Casos")) {
                renderCasosTab(experiment);
                ImGui::EndTabItem();
            }
            if (ImGui::BeginTabItem("Hipoteses")) {
                renderHipotesesTab(experiment);
                ImGui::EndTabItem();
            }
            if (ImGui::BeginTabItem("Experimentos")) {
                renderExperimentosTab(experiment);
                ImGui::EndTabItem();
            }
            if (ImGui::BeginTabItem("Evidencias")) {
                renderEvidenciasTab(experiment);
                ImGui::EndTabItem();
            }
            if (ImGui::BeginTabItem("Decisao")) {
                renderDecisaoTab(report);
                ImGui::EndTabItem();
            }
            ImGui::EndTabBar();
        }

        ImGui::Separator();
        if (ImGui::Button("Registrar Rodada Demo em ./output/lv-round-001")) {
            try {
                const RoundArtifacts artifacts = recordVirtualLabRound("output/lv-round-001");
                (void)artifacts;
            } catch (const std::exception& ex) {
                std::fprintf(stderr, "recordVirtualLabRound failed: %s\n", ex.what());
            }
        }
        ImGui::SameLine();
        ImGui::TextDisabled("Shell inicial do Laboratorio Virtual");

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
