#pragma once

#include <juce_gui_extra/juce_gui_extra.h>

class Timeline final : public juce::Component
{
public:
    explicit Timeline(int numOfBoxes);
    ~Timeline() override = default;

    void paint(juce::Graphics& g) override;
    void resized() override;

private:
    juce::FlexBox timelineFlexBox;
    const int tempNumOfSeconds{};
};