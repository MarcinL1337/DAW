#pragma once

#include <juce_gui_extra/juce_gui_extra.h>
#include "../Constants.h"

class TrackGui : public juce::Component
{
public:
    explicit TrackGui(int numOfBoxes, int id = -1);
    TrackGui(const TrackGui&) = delete;
    TrackGui& operator=(const TrackGui&) = delete;
    ~TrackGui() override = default;

    void paint(juce::Graphics& g) override;
    void resized() override;

    float getGridBoxWidth() const { return currentGridBoxWidth; }
    float getGridBoxHeight() const { return currentGridBoxHeight; }

private:
    const float currentGridBoxWidth{TrackPlayerConstants::startBoxWidth};
    const float currentGridBoxHeight{TrackPlayerConstants::startBoxHeight};
    const int currentNumOfBoxes{};
    const int id;
};