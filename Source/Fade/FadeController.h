#pragma once

#include <juce_audio_utils/juce_audio_utils.h>
#include "../Constants.h"
#include "Fade.h"

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

    float getFadeMultiplier(double timePositionSeconds, double totalLengthSeconds) const;
    bool hasFade() const;

private:
    juce::Path buildFadePath(bool isFadeIn, int width, int height);
    void showFunctionMenu(bool isFadeIn);
    void notifyAudioProcessor();
    void rebuildPaths();
    juce::Point<int> getHandlePosition(bool isFadeIn) const;
    void drawHandle(juce::Graphics& g, bool isFadeIn) const;
    bool isMouseOverHandle(const juce::Point<int>& mousePos, bool isFadeIn) const;

    Fade::Data& getFadeData(const bool isFadeIn) { return isFadeIn ? fadeIn : fadeOut; }
    const Fade::Data& getFadeData(const bool isFadeIn) const { return isFadeIn ? fadeIn : fadeOut; }

    juce::ValueTree& tree;
    NodeID audioClipID;
    Fade::Data fadeIn, fadeOut;
    juce::Optional<bool> draggingHandle;
    uint16_t currentBoxWidth{TrackPlayerConstants::startBoxWidth};
    const int handleSize{8};
    const int mouseInteractionDistance = 10;
    float audioLengthSeconds{0.0f};

    friend class Waveform;
};
