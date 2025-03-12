#pragma once

#include <juce_gui_extra/juce_gui_extra.h>
#include "TimeBar.h"

class Timeline final : public juce::Component
{
public:
    explicit Timeline(int numOfBoxes);
    ~Timeline() override = default;

    void paint(juce::Graphics& g) override;
    void resized() override;

private:
    const int tempNumOfSeconds{};
    juce::Point<int> lastMousePosition{};
    bool isCurrentlyDraggingTimeBar{false};
};