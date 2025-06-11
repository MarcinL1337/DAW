#pragma once

#include <juce_audio_utils/juce_audio_utils.h>
#include "../Constants.h"

using NodeID = juce::AudioProcessorGraph::NodeID;

class FadeController final : public juce::Component
{
public:
    explicit FadeController(juce::ValueTree& parentTree, NodeID audioClipID);

    void updateForNewAudioLength(float audioLengthSeconds);
    void updateForNewBoxWidth(uint16_t newBoxWidth);

protected:
    void paint(juce::Graphics& g) override;
    void resized() override;
    void mouseDown(const juce::MouseEvent& event) override;
    void mouseDrag(const juce::MouseEvent& event) override;
    void mouseUp(const juce::MouseEvent& event) override;
    void mouseMove(const juce::MouseEvent& event) override;
    bool hitTest(int x, int y) override;

private:
    juce::Path buildFadePath(bool isFadeIn, int width, int height);
    void showFunctionMenu(bool isFadeIn);
    void notifyAudioProcessor();
    void rebuildPaths();
    juce::Point<int> getHandlePosition(bool isFadeIn) const;
    void drawHandle(juce::Graphics& g, bool isFadeIn) const;
    bool isMouseOverHandle(const juce::Point<int>& mousePos, bool isFadeIn) const;

    Fade::Data& getFadeData(const bool isFadeIn) { return fadeData[isFadeIn ? 0 : 1]; }
    const Fade::Data& getFadeData(const bool isFadeIn) const { return fadeData[isFadeIn ? 0 : 1]; }

    juce::ValueTree& tree;
    NodeID audioClipID;
    std::array<Fade::Data, 2> fadeData;  // [0] = fade in, [1] = fade out
    juce::Optional<bool> draggingHandle;
    uint16_t currentBoxWidth{TrackPlayerConstants::startBoxWidth};
    const int handleSize{8};
};
