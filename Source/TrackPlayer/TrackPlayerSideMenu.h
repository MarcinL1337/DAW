#pragma once

#include <juce_gui_extra/juce_gui_extra.h>
#include "../Constants.h"

class TrackPlayerSideMenu final : public juce::Component
{
public:
    TrackPlayerSideMenu() = default;
    ~TrackPlayerSideMenu() override = default;

    void paint(juce::Graphics& g) override;
    void resized() override;

    uint16_t getCurrentNumberOfTracks() const { return currentNumberOfTracks; }
    void incrementCurrentNumberOfTracks() { currentNumberOfTracks++; }

private:
    uint16_t currentNumberOfTracks{1};
};