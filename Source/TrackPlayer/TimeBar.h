#pragma once

#include <juce_gui_extra/juce_gui_extra.h>

class TimeBar final : public juce::Component
{
public:
    TimeBar();
    ~TimeBar() override = default;

    void paint(juce::Graphics& g) override;
    void resized() override;
};