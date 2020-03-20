//
// Created by mkalte on 20/03/2020.
//

#include "fftview.h"
#include "dsp/hamming.h"
void FftView::update(AudioHandler& audioHandler)
{
    if (audioHandler.getFrameCount() > lastFrame) {
        lastFrame = audioHandler.getFrameCount();
        audioHandler.getFrame(reference, input);
        hamming(reference);
        hamming(input);
        refFft = fftReal(reference);
        inFft = fftReal(input);
        H.resize(inFft.size());
        refFftAvg.resize(inFft.size());
        inFftAvg.resize(inFft.size());
        for (size_t i = 0ul; i < inFft.size(); i++) {
            refFftAvg[i] = (refFftAvg[i] + refFft[i]) / 2.0;
            inFftAvg[i] = (inFftAvg[i] + inFft[i]) / 2.0;
            H[i] = inFftAvg[i] / refFftAvg[i];
        }
        h = ifftReal(H);
    }

    ImGui::Begin("FFT View");
    ImGui::PlotLines(
        "Mag", [](void* data, int idx) {
            auto p = reinterpret_cast<FftView*>(data);
            return static_cast<float>(std::abs(p->inFftAvg[static_cast<size_t>(idx)]));
        },
        this, static_cast<int>(inFftAvg.size() / 2));

    ImGui::PlotLines(
        "H", [](void* data, int idx) {
            auto p = reinterpret_cast<FftView*>(data);
            return static_cast<float>(std::abs(p->H[static_cast<size_t>(idx)].real()));
        },
        this, static_cast<int>(H.size() / 2), 0, nullptr, 0.0F, 2.0F);

    ImGui::PlotLines(
        "h", [](void* data, int idx) {
            auto p = reinterpret_cast<FftView*>(data);
            return static_cast<float>(p->h[static_cast<size_t>(idx)]);
        },
        this, static_cast<int>(h.size() / 2));
    ImGui::End();
}
