#include "TrackPlayerSideMenu.h"

void TrackPlayerSideMenu::paint(juce::Graphics& g)
{
    g.setColour(juce::Colours::lightgrey);
    g.drawRect(getLocalBounds());
}

void TrackPlayerSideMenu::resized() {}
