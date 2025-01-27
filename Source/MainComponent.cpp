#include "MainComponent.h"

MainComponent::MainComponent()
{
    topLevelMenu = std::make_unique<Menu>();
    addAndMakeVisible(topLevelMenu.get());
    mainToolbar = std::make_unique<MainToolbar>();
    mainToolbar->setBounds(0, 50, getWidth(), 100);
    addAndMakeVisible(mainToolbar.get());
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

    // topLevelMenu->setBounds(topLevelMenu->getXPosition(), topLevelMenu->getYPosition(), topLevelMenu->getMenuWidth(), topLevelMenu->getMenuHeight());
    // mainToolbar.setBounds(0, topLevelMenu->getYPosition(), getWidth(), 100);

    topLevelMenu->setBounds(0, 0, getWidth(), 50);
    mainToolbar->setBounds(0, 50, getWidth(), 50);

}

