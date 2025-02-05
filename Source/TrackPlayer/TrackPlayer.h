#pragma once

#include <juce_gui_extra/juce_gui_extra.h>
#include "ClipsBoxes.h"

class TrackPlayer final : public juce::Component
{
public:
    TrackPlayer();
    ~TrackPlayer() override = default;

    void paint(juce::Graphics& g) override;
    void resized() override;

    void flexBoxInit();

private:
    juce::FlexBox clipsBoxes;
    juce::Array<ClipsBox> clipsBoxesArray{};
};