#include "MainComponent.h"

MainComponent::MainComponent()
{
    addAndMakeVisible(topLevelMenu);
    addAndMakeVisible(mainToolbar);
    flexBoxInit();
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
    mainFlexBox.performLayout(getLocalBounds());
}

void MainComponent::flexBoxInit()
{
    mainFlexBox.flexDirection = juce::FlexBox::Direction::column;
    mainFlexBox.flexWrap = juce::FlexBox::Wrap::noWrap;

    mainFlexBox.items.add(juce::FlexItem(0, topLevelMenuHeight, topLevelMenu));
    mainFlexBox.items.add(juce::FlexItem(0, mainToolbarHeight, mainToolbar));
    mainFlexBox.items.add(juce::FlexItem(mainContentFlexBox).withFlex(1.0));
    mainContentFlexBox.items.add(juce::FlexItem(mainWindowFlexBox).withFlex(1.0));
}
