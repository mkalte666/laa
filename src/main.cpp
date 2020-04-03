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

int main(int, char**)
{
    srand(static_cast<unsigned int>(time(nullptr)));
    auto res = s2::SDL2::init(s2::InitFlags::Audio | s2::InitFlags::Video);
    if (!res) {
        std::cerr << "SDL Init  failed : " << res.getErrorMessage() << "\n";
        return -1;
    }

#ifndef _WIN32
    s2::Video::GL::setAttribute(s2::GLattr::ContextFlags, 0);
    s2::Video::GL::setAttribute(s2::GLattr::ContextProfileMask, SDL_GL_CONTEXT_PROFILE_ES);
    s2::Video::GL::setAttribute(s2::GLattr::ContextMajorVersion, 2);
    s2::Video::GL::setAttribute(s2::GLattr::ContextMinorVersion, 0);
    s2::Video::GL::setAttribute(s2::GLattr::Doublebuffer, 1);
    const char* glsl_version = "#version 100";
#else
    s2::Video::GL::setAttribute(s2::GLattr::ContextFlags, 0);
    s2::Video::GL::setAttribute(s2::GLattr::ContextProfileMask, SDL_GL_CONTEXT_PROFILE_CORE);
    s2::Video::GL::setAttribute(s2::GLattr::ContextMajorVersion, 3);
    s2::Video::GL::setAttribute(s2::GLattr::ContextMinorVersion, 0);
    s2::Video::GL::setAttribute(s2::GLattr::Doublebuffer, 1);
    const char* glsl_version = "#version 130";
#endif
    auto windowRes = s2::Video::Window::createCentered("LAA", 1024, 768, s2::WindowFlags::Resizable | s2::WindowFlags::Opengl);
    if (!windowRes) {
        std::cerr << "OpenGL Window creation failed : " << windowRes.getErrorMessage() << "\n";
        return -1;
    }
    auto window = windowRes.extractValue();
    auto contextRes = s2::Video::GL::Context::create(window);
    if (!contextRes) {
        std::cerr << "OpenGL Context creation failed : " << contextRes.getErrorMessage() << "\n";
        return -1;
    }
    auto context = contextRes.extractValue();
    s2::Video::GL::setSwapInterval(1);

    // imgui
    auto gl3wres = gl3wInit2(reinterpret_cast<GL3WGetProcAddressProc>(&SDL_GL_GetProcAddress));
    (void)gl3wres;
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGui_ImplSDL2_InitForOpenGL(window.get(), context.get());
    ImGui_ImplOpenGL3_Init(glsl_version);

    bool running = true;

    // view manager takes quite a while, so show some "wait a second" text
    // its in a loop so we actually get this out after the window is created and before we init
    // this is also the reason for the delay
    s2::Timer::delay(200);
    for (int i = 0; i < 10; i++) {
        s2::Event e;
        while (s2::Input::pollEvent(e)) {
            if (ImGui_ImplSDL2_ProcessEvent(&e)) {
                continue;
            }
            if (e.type == s2::EventType::Quit) {
                running = false;
            }
        }
        glClearColor(0.0F, 0.0F, 0.0F, 1.0F);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // NOLINT

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplSDL2_NewFrame(window.get());
        ImGui::NewFrame();
        ImGui::Begin("Waiting info window", nullptr, ImGuiWindowFlags_AlwaysAutoResize);
        ImGui::Text("Setting up. This might take a while.");
        ImGui::Text("Im sorry for this, but everything should be alot smoother as soon as we get going!");
        ImGui::End();
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        s2::Video::GL::swap(window);
    }

    // the blob above is there cause of this one
    auto manager = std::make_unique<ViewManager>();

    while (running) {
        s2::Event e;
        while (s2::Input::pollEvent(e)) {
            if (ImGui_ImplSDL2_ProcessEvent(&e)) {
                continue;
            }
            if (e.type == s2::EventType::Quit) {
                running = false;
            }
        }

        glClearColor(0.0F, 0.0F, 0.0F, 1.0F);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // NOLINT

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplSDL2_NewFrame(window.get());
        ImGui::NewFrame();
        int w, h;
        s2::Video::GL::getDrawableSize(window, w, h);
        manager->update(ImVec2(static_cast<float>(w), static_cast<float>(h)));

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        s2::Video::GL::swap(window);
    }

    return 0;
}