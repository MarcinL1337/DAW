#include "SideMenu.h"

SideMenu::SideMenu() { initSliders(); }

void SideMenu::paint(juce::Graphics& g)
{
    g.setColour(juce::Colours::whitesmoke);
    g.drawRect(getLocalBounds());
}

void SideMenu::resized() { positionSliders(); }

void SideMenu::initSliders()
{
    addAndMakeVisible(faderSlider);
    faderSlider.setRange(-15, 15);
    faderSlider.setTextValueSuffix(" dB");
    faderSlider.

        addAndMakeVisible(faderLabel);
    faderLabel.setText("Fader", juce::dontSendNotification);
    faderLabel.attachToComponent(&faderSlider, false);
}

void SideMenu::positionSliders()
{
    faderSlider.setSize(getWidth() * 0.95, sliderHeight);
    faderSlider.setCentrePosition(getWidth() * 0.5, 50);
}
