#include "SideMenu.h"

void SideMenu::paint(juce::Graphics& g)
{
    g.setColour(juce::Colours::lightgrey);
    g.drawRect(getLocalBounds());
    std::cout << "SideMenu: " << getLocalBounds().getHeight() << " x " << getLocalBounds().getWidth() << std::endl;
}

void SideMenu::resized() {}