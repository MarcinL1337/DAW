#include "TrackPlayerSideMenu.h"

void TrackPlayerSideMenu::paint(juce::Graphics& g)
{
    g.setColour(juce::Colours::lightgrey);

    for(auto i{0u}; i < TrackPlayerConstants::startNumOfBoxesRows; i++)
    {
        g.drawLine(
            0, i * TrackPlayerConstants::startBoxHeight, getWidth(), i * TrackPlayerConstants::startBoxHeight, 0.75);
    }
}

void TrackPlayerSideMenu::resized() {}
