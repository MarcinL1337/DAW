#include "MainComponent.h"

MainComponent::MainComponent()
{
    addAndMakeVisible(topLevelMenu);
    addAndMakeVisible(mainToolbar);
}

void MainComponent::paint(juce::Graphics& g)
{
    g.fillAll(getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId));
}

void MainComponent::resized()
{
    // This is called when the MainComponent is resized.
    // If you add any child components, this is where you should
    // update their positions.

    topLevelMenu.setBounds(
        getLocalBounds().removeFromTop(juce::LookAndFeel::getDefaultLookAndFeel().getDefaultMenuBarHeight()));
    mainToolbar.setBounds(0, 25, getWidth(), 45);
}

