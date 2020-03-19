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

int main(int, char**)
{
    auto res = s2::SDL2::init(s2::InitFlags::Audio | s2::InitFlags::Video);
    if (!res) {
        return -1;
    }

    s2::Video::GL::setAttribute(s2::GLattr::ContextFlags, 0);
    s2::Video::GL::setAttribute(s2::GLattr::ContextProfileMask, SDL_GL_CONTEXT_PROFILE_ES);
    s2::Video::GL::setAttribute(s2::GLattr::ContextMajorVersion, 2);
    s2::Video::GL::setAttribute(s2::GLattr::ContextMinorVersion, 0);
    s2::Video::GL::setAttribute(s2::GLattr::Doublebuffer, 1);

    auto windowRes = s2::Video::Window::createCentered("LAA", 1024, 768, s2::WindowFlags::Resizable | s2::WindowFlags::Opengl);
    if (!windowRes) {
        return -1;
    }
    auto window = windowRes.extractValue();
    auto contextRes = s2::Video::GL::Context::create(window);
    if (!contextRes) {
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
    ImGui_ImplOpenGL3_Init("#version 100");

    bool running = true;
    ViewManager manager;

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

        manager.update();

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        SDL_GL_SwapWindow(window.get());
    }

    return 0;
}