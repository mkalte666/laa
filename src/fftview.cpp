//
// Created by mkalte on 20/03/2020.
//

#include "fftview.h"
void FftView::update(AudioHandler& audioHandler)
{
    if (audioHandler.getFrameCount() > lastFrame) {
        lastFrame = audioHandler.getFrameCount();
        audioHandler.getFrame(reference, input);
        refFft = fftReal(reference);
        inFft = fftReal(input);
        H.resize(inFft.size());
        for (size_t i = 0ul; i < inFft.size(); i++) {
            H[i] = inFft[i] / refFft[i];
        }
        h = ifftReal(H);
    }

    ImGui::Begin("FFT View");
    ImGui::PlotLines(
        "Mag", [](void* data, int idx) {
            auto p = reinterpret_cast<FftView*>(data);
            return static_cast<float>(std::abs(p->inFft[static_cast<size_t>(idx)]));
        },
        this, static_cast<int>(inFft.size() / 2));

    ImGui::PlotLines(
        "H", [](void* data, int idx) {
            auto p = reinterpret_cast<FftView*>(data);
            return static_cast<float>(std::abs(p->H[static_cast<size_t>(idx)]));
        },
        this, static_cast<int>(H.size() / 2));

    ImGui::PlotLines(
        "h", [](void* data, int idx) {
            auto p = reinterpret_cast<FftView*>(data);
            return static_cast<float>(p->h[static_cast<size_t>(idx)]);
        },
        this, static_cast<int>(h.size()));
    ImGui::End();
}
