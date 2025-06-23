#pragma once

#include <juce_gui_extra/juce_gui_extra.h>

class ClipSplitBar final : public juce::Component
{
public:
    ClipSplitBar();
    ~ClipSplitBar() override = default;

    void setMouseXPosition(const float newMouseXPosition) { mouseXPosition = newMouseXPosition; }

private:
    void paint(juce::Graphics& g) override;
    void resized() override;

    float mouseXPosition{};
};