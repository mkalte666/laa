/*
 * This file is part of LAA
 * Copyright (c) 2020 Malte Kießling
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, version 3.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#include "shared.h"
#include "viewmanager.h"
#include <ctime>
#include <iostream>
#include <memory>
#include <random>

static constexpr int startDelay = 300;
static constexpr int initialLoopsBeforeLongLoad = 10;
static constexpr int minimalWindowWidth = 320;
static constexpr int minimalWindowHeight = 240;

int main(int, char**)
{
    // we just need "random", not random
    // NOLINTNEXTLINE
    srand(static_cast<unsigned int>(time(nullptr)));
    auto res = SDL_Init(SDL_INIT_AUDIO | SDL_INIT_VIDEO);
    if (res != 0) {
        std::cerr << "SDL Init  failed : " << SDL_GetError() << "\n";
        return -1;
    }

#ifdef LAA_GL_ES_2
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, 0);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_ES);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    const char* glsl_version = "#version 100";
#else
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, 0);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    const char* glsl_version = "#version 130";
#endif
    //windowpos is doing shitstuff NOLINTNEXTLINE
    auto window = SDL_CreateWindow("LAA", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1024, 768, SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);
    if (window == nullptr) {
        std::cerr << "OpenGL Window creation failed : " << SDL_GetError() << "\n";
        return -1;
    }

    auto* context = SDL_GL_CreateContext(window);
    if (context == nullptr) {
        std::cerr << "OpenGL Context creation failed : " << SDL_GetError() << "\n";
        SDL_DestroyWindow(window);
        SDL_Quit();
        return -1;
    }

    SDL_GL_SetSwapInterval(1);

    // imgui
    // glew has a slightly different signature, but they are compatible NOLINTNEXTLINE
    auto gl3wres = gl3wInit2(reinterpret_cast<GL3WGetProcAddressProc>(&SDL_GL_GetProcAddress));
    (void)gl3wres;
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGui_ImplSDL2_InitForOpenGL(window, context);
    ImGui_ImplOpenGL3_Init(glsl_version);
    bool running = true;

    // view manager takes quite a while, so show some "wait a second" text
    // its in a loop so we actually get this out after the window is created and before we init
    // this is also the reason for the delay
    SDL_Delay(startDelay);
    for (int i = 0; i < initialLoopsBeforeLongLoad; i++) {
        SDL_Event e;
        while (SDL_PollEvent(&e) != 0) {
            if (ImGui_ImplSDL2_ProcessEvent(&e)) {
                continue;
            }
            if (e.type == SDL_QUIT) {
                running = false;
            }
        }
        glClearColor(0.0F, 0.0F, 0.0F, 1.0F); // NOLINT
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // NOLINT

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplSDL2_NewFrame(window);
        ImGui::NewFrame();
        ImGui::Begin("Waiting info window", nullptr, ImGuiWindowFlags_AlwaysAutoResize);
        ImGui::Text("Setting up. This might take a while."); //NOLINT
        ImGui::Text("Im sorry for this, but everything should be alot smoother as soon as we get going!"); //NOLINT
        ImGui::End();
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        SDL_GL_SwapWindow(window);
    }

    // the blob above is there cause of this one
    auto manager = std::make_unique<ViewManager>();

    while (running) {
        SDL_Event e;
        while (SDL_PollEvent(&e) != 0) {
            if (ImGui_ImplSDL2_ProcessEvent(&e)) {
                continue;
            }
            if (e.type == SDL_QUIT) {
                running = false;
            }
        }

        glClearColor(0.0F, 0.0F, 0.0F, 1.0F); // NOLINT
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // NOLINT

        // make sure window size is not null
        // sadly window.setminsize can and is ingored by some wm on linux
        int windowW = 0;
        int windowH = 0;
        SDL_GetWindowSize(window, &windowW, &windowH);

        if (windowW < minimalWindowWidth || windowH < minimalWindowHeight) {
            windowW = std::max(minimalWindowWidth, windowW);
            windowH = std::max(minimalWindowHeight, windowH);
            SDL_SetWindowSize(window, windowW, windowH);
        }

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplSDL2_NewFrame(window);
        ImGui::NewFrame();

        int w = 0;
        int h = 0;
        SDL_GL_GetDrawableSize(window, &w, &h);
        manager->update(ImVec2(static_cast<float>(w), static_cast<float>(h)));

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        SDL_GL_SwapWindow(window);
    }

    SDL_GL_DeleteContext(context);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}