#include "MainComponent.h"

//==============================================================================
MainComponent::MainComponent()
{
    setSize (1600, 1000);
    menuBar = std::make_unique<MenuBar>();
    addAndMakeVisible(menuBar.get());
}

//==============================================================================
void MainComponent::paint (juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));

    const juce::FontOptions mainComponentFont{"Comic Sans Serif", 20.0f, juce::Font::bold | juce::Font::italic};
    g.setFont (mainComponentFont);
    g.setColour (juce::Colours::white);
    g.drawText ("Hello World!", getLocalBounds(), juce::Justification::centred, true);

}

void MainComponent::resized()
{
    // This is called when the MainComponent is resized.
    // If you add any child components, this is where you should
    // update their positions.

    // menuBar->setBounds(100, 100, getWidth(), 50);
}
