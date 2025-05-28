#pragma once

#include <juce_gui_extra/juce_gui_extra.h>
#include "../Constants.h"
#include "FadeHandle.h"
#include "FadeRenderer.h"
#include "FadeMouseHandler.h"

class FadeController final : public juce::Component
{
public:
    explicit FadeController(juce::ValueTree& parentTree, NodeID audioClipID);

    void setFadeLength(bool isFadeIn, float lengthInSeconds);
    void setFadeType(bool isFadeIn, FadeType type);
    void updateForNewAudioLength(float audioLengthSeconds);
    void updateForNewBoxWidth(uint16_t newBoxWidth);

    float getFadeLength(bool isFadeIn) const;
    FadeType getFadeType(bool isFadeIn) const;
    bool isFadeEnabled(bool isFadeIn) const;

    const FadeHandle& getFadeHandle(bool isFadeIn) const;

protected:
    void paint(juce::Graphics& g) override;
    void resized() override;
    void mouseDown(const juce::MouseEvent& event) override;
    void mouseDrag(const juce::MouseEvent& event) override;
    void mouseUp(const juce::MouseEvent& event) override;
    void mouseMove(const juce::MouseEvent& event) override;

private:
    void showTypeMenu(bool isFadeIn);
    void notifyAudioProcessor();
    void updateMouseCursor();
    void rebuildPathsIfNeeded();

    juce::ValueTree& tree;
    NodeID audioClipID;

    FadeHandle fadeIn{true};
    FadeHandle fadeOut{false};
    FadeMouseHandler mouseHandler{fadeIn, fadeOut};

    uint16_t currentBoxWidth{TrackPlayerConstants::startBoxWidth};
    float currentAudioLength{0.0f};

    bool needsRepaint{false};

    static juce::PopupMenu createTypeMenu(FadeType currentType);
    static void handleTypeMenuResult(int result, bool isFadeIn, FadeController* controller);
};
