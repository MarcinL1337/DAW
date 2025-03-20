#pragma once

#include <juce_gui_extra/juce_gui_extra.h>
#include "../Constants.h"
#include "TimeBar.h"

class Timeline final : public juce::Component
{
public:
    explicit Timeline(int numOfBoxes);
    ~Timeline() override = default;

    void paint(juce::Graphics& g) override;
    void resized() override;

    void mouseDown(const juce::MouseEvent& event) override;
    void mouseDrag(const juce::MouseEvent& event) override;
    void mouseUp(const juce::MouseEvent& event) override;

private:
    TimeBar timeBar{};

    int timeBarXOffset{0};
    const int tempNumOfSeconds{};
    juce::Point<int> lastMousePosition{};
    bool isCurrentlyDraggingTimeBar{false};
};