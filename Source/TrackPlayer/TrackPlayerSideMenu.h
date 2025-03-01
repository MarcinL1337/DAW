#pragma once

#include <juce_gui_extra/juce_gui_extra.h>
#include "../Constants.h"

class TrackPlayerSideMenu : public juce::Component
{
public:
    TrackPlayerSideMenu() = default;
    ~TrackPlayerSideMenu() override = default;

    void paint(juce::Graphics& g) override;
    void resized() override;
};