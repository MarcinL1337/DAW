#pragma once

#include <juce_gui_extra/juce_gui_extra.h>

class TrackPlayer final : public juce::Component
{
public:
    TrackPlayer() = default;
    ~TrackPlayer() override = default;

    void paint(juce::Graphics& g) override;
    void resized() override;

private:
    juce::FlexBox trackPlayerFlexBox;
};