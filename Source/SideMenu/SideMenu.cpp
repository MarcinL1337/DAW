#include "SideMenu.h"

void SideMenu::paint(juce::Graphics& g)
{
    g.setColour(juce::Colours::lightgrey);
    g.drawRect(getLocalBounds());
}

void SideMenu::resized() {}