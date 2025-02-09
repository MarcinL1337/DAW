#include "ClipsBoxes.h"

ClipsBox::ClipsBox(const float x_coord, const float y_coord, int numOfBoxes) :
    x(x_coord), y(y_coord), currentNumOfBoxes{numOfBoxes}
{}

void ClipsBox::paint(juce::Graphics& g)
{
    g.setColour(juce::Colours::lightgrey);

    for(int i{0}; i < currentNumOfBoxes; i++)
    {
        g.drawRect(i * TrackPlayerConstants::startBoxWidth,
                   y,
                   TrackPlayerConstants::startBoxWidth,
                   TrackPlayerConstants::startBoxHeight,
                   0.75);
    }
}

void ClipsBox::resized() {}
