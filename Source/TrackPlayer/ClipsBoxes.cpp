#include "ClipsBoxes.h"

ClipsBox::ClipsBox(const float x_coord, const float y_coord) : x(x_coord), y(y_coord) {}

void ClipsBox::paint(juce::Graphics& g)
{
    const auto parentWidth{getWidth()};
    const auto startAmountOfBoxes{ceil(parentWidth / TrackPlayerConstants::startBoxWidth)};

    g.setColour(juce::Colours::lightgrey);

    for(int i{0}; i < static_cast<int>(startAmountOfBoxes) * 2; i++)
    {
        g.drawRect(i * TrackPlayerConstants::startBoxWidth,
                   y,
                   TrackPlayerConstants::startBoxWidth,
                   TrackPlayerConstants::startBoxHeight,
                   0.75);
    }
}

void ClipsBox::resized() {}
