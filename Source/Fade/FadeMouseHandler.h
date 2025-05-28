#pragma once

#include <juce_gui_extra/juce_gui_extra.h>
#include "FadeHandle.h"

class FadeMouseHandler
{
public:
    FadeMouseHandler(FadeHandle& fadeIn, FadeHandle& fadeOut);

    bool handleMouseDown(const juce::MouseEvent& event, int componentWidth, uint16_t boxWidth, int componentHeight);
    bool handleMouseDrag(const juce::MouseEvent& event, uint16_t boxWidth, float currentAudioLength);
    bool handleMouseUp(const juce::MouseEvent& event);
    bool handleMouseMove(const juce::MouseEvent& event, int componentWidth, uint16_t boxWidth, int componentHeight);

    FadeHandle* getActiveHandle();
    const FadeHandle* getActiveHandle() const;
    bool hasActiveHandle() const;

    juce::MouseCursor getAppropriateMouseCursor() const;

    void reset();

private:
    FadeHandle& fadeIn;
    FadeHandle& fadeOut;

    bool updateMouseOverStates(const juce::Point<int>& mousePos, int componentWidth, uint16_t boxWidth,
                               int componentHeight);

    void updateFadeLength(FadeHandle& handle, float newLength, float maxLength);
};
