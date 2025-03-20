#include "MainToolbar.h"

MainToolbar::MainToolbar(MainAudio& mainAudioRef) : toolbarFactory(mainAudioRef)
{
    addAndMakeVisible(toolbar);
    toolbar.addDefaultItems(toolbarFactory);
}

void MainToolbar::resized() { toolbar.setBounds(getLocalBounds()); }

void MainToolbar::paint(juce::Graphics& g)
{
    g.setColour(juce::Colours::black);
    g.drawRect(getLocalBounds());

    // TODO: this is gonna be a place where current TimeBar position is displayed in the ValueTree introduction commit
    g.setColour(juce::Colours::whitesmoke);
    g.drawRect(getWidth() - 200, 2, 200 - 2, getHeight() - 4, 2);
}