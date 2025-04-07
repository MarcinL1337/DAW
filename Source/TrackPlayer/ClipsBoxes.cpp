#include "ClipsBoxes.h"

ClipsBox::ClipsBox(const int numOfBoxes, const int id) : currentNumOfBoxes{numOfBoxes}, id{id} {}

void ClipsBox::paint(juce::Graphics& g)
{
    g.setColour(juce::Colours::lightgrey);

    for(auto i{0u}; i < currentNumOfBoxes; i++)
    {
        // TODO: take this into consideration: https://forum.juce.com/t/best-practice-for-2d-graph-scrolling/13359/5
        g.drawRect(i * TrackPlayerConstants::startBoxWidth,
                   0.0,
                   TrackPlayerConstants::startBoxWidth,
                   TrackPlayerConstants::startBoxHeight,
                   0.75);
    }
}

void ClipsBox::resized() {}
