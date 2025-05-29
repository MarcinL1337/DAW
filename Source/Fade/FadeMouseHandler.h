#pragma once

#include "FadeHandle.h"

class FadeMouseHandler
{
public:
    FadeMouseHandler(FadeHandle& fadeIn, FadeHandle& fadeOut);

    bool handleMouseDown(const juce::MouseEvent& event, int componentWidth, uint16_t boxWidth,
                         int componentHeight) const;
    bool handleMouseDrag(const juce::MouseEvent& event, uint16_t boxWidth, float currentAudioLength);
    bool handleMouseUp(const juce::MouseEvent& event) const;
    bool handleMouseMove(const juce::MouseEvent& event, int componentWidth, uint16_t boxWidth,
                         int componentHeight) const;

    FadeHandle* getActiveHandle()
    {
        return fadeIn.getDragging() ? &fadeIn : (fadeOut.getDragging() ? &fadeOut : nullptr);
    }
    const FadeHandle* getActiveHandle() const
    {
        return fadeIn.getDragging() ? &fadeIn : (fadeOut.getDragging() ? &fadeOut : nullptr);
    }

    juce::MouseCursor getAppropriateMouseCursor() const;
    void reset() const;

private:
    FadeHandle& fadeIn;
    FadeHandle& fadeOut;

    bool updateMouseOverStates(const juce::Point<int>& mousePos, int componentWidth, uint16_t boxWidth,
                               int componentHeight) const;
};
