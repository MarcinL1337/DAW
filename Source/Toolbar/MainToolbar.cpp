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
}