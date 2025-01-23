#include "MainComponent.h"

MainComponent::MainComponent()
{
    menuBar = std::make_unique<Menu>();
    addAndMakeVisible(menuBar.get());
}

void MainComponent::paint (juce::Graphics& g)
{
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));
}

void MainComponent::resized()
{
    // This is called when the MainComponent is resized.
    // If you add any child components, this is where you should
    // update their positions.

    menuBar->setBounds(0, 0, getWidth(), 50);
}
