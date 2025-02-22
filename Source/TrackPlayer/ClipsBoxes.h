#pragma once

#include <juce_gui_extra/juce_gui_extra.h>
#include "../Constants.h"

class ClipsBox : public juce::Component
{
public:
    ClipsBox(int numOfBoxes);
    ClipsBox(const ClipsBox&) = delete;
    ClipsBox& operator=(const ClipsBox&) = delete;
    ~ClipsBox() override = default;

    void paint(juce::Graphics& g) override;
    void resized() override;

    float getGridBoxWidth() const { return currentGridBoxWidth; }
    float getGridBoxHeight() const { return currentGridBoxHeight; }

private:
    const float currentGridBoxWidth{TrackPlayerConstants::startBoxWidth};
    const float currentGridBoxHeight{TrackPlayerConstants::startBoxHeight};
    const int currentNumOfBoxes{};
};