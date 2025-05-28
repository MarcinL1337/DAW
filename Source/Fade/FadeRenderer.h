#pragma once

#include <juce_gui_extra/juce_gui_extra.h>
#include "FadeHandle.h"

class FadeRenderer
{
public:
    static void drawFadeHandle(juce::Graphics& g, const FadeHandle& handle,
                              int componentWidth, uint16_t boxWidth, int componentHeight);

    static void drawFadePath(juce::Graphics& g, const FadeHandle& handle);

    static void drawAllFades(juce::Graphics& g, const FadeHandle& fadeIn,
                            const FadeHandle& fadeOut, int componentWidth,
                            uint16_t boxWidth, int componentHeight);

    static juce::Colour getFadeInColour() { return juce::Colours::red.withAlpha(0.4f); }
    static juce::Colour getFadeOutColour() { return juce::Colours::blue.withAlpha(0.4f); }
    static juce::Colour getHandleColour(bool isMouseOver);
    static juce::Colour getHandleBorderColour() { return juce::Colours::black; }

private:
    static constexpr int handleSize{10};
    static constexpr int handleBorderThickness{1};

    static void drawSingleFadePath(juce::Graphics& g, const FadeHandle& handle, juce::Colour colour);
    static void drawSingleHandle(juce::Graphics& g, const FadeHandle& handle,
                                int componentWidth, uint16_t boxWidth, int componentHeight);
};
