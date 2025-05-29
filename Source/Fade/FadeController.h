#pragma once
#include <juce_audio_utils/juce_audio_utils.h>
#include "FadeHandle.h"

using NodeID = juce::AudioProcessorGraph::NodeID;

class FadeController final : public juce::Component
{
public:
    explicit FadeController(juce::ValueTree& parentTree, NodeID audioClipID);

    void setFadeType(bool isFadeIn, FadeType type);
    void updateForNewAudioLength(float audioLengthSeconds);
    void updateForNewBoxWidth(uint16_t newBoxWidth);
    FadeType getFadeType(bool isFadeIn) const;

protected:
    void paint(juce::Graphics& g) override;
    void resized() override;
    void mouseDown(const juce::MouseEvent& event) override;
    void mouseDrag(const juce::MouseEvent& event) override;
    void mouseUp(const juce::MouseEvent& event) override;
    void mouseMove(const juce::MouseEvent& event) override;
    bool hitTest(int x, int y) override;

private:
    void showTypeMenu(bool isFadeIn);
    void notifyAudioProcessor();
    void updateMouseCursor();
    void rebuildPathsIfNeeded();
    FadeHandle* getActiveHandle();
    bool updateMouseOverStates(const juce::Point<int>& mousePos);

    FadeHandle& getHandle(const bool isFadeIn) { return isFadeIn ? fadeIn : fadeOut; }
    const FadeHandle& getHandle(const bool isFadeIn) const { return isFadeIn ? fadeIn : fadeOut; }

    juce::ValueTree& tree;
    NodeID audioClipID;
    FadeHandle fadeIn{true};
    FadeHandle fadeOut{false};
    uint16_t currentBoxWidth{TrackPlayerConstants::startBoxWidth};
    float currentAudioLength{0.0f};
};