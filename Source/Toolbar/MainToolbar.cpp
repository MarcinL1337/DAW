#include "MainToolbar.h"

MainToolbar::MainToolbar(MainAudio& mainAudioRef, const juce::ValueTree& parentTree) :
    tree{parentTree}, toolbarFactory(mainAudioRef)
{
    addAndMakeVisible(toolbar);
    toolbar.addDefaultItems(toolbarFactory);
    tree.addListener(this);
}

void MainToolbar::resized() { toolbar.setBounds(getLocalBounds()); }

void MainToolbar::paint(juce::Graphics& g)
{
    g.setColour(juce::Colours::black);
    g.drawRect(getLocalBounds());

    // TODO: style this better and refactor
    g.setColour(juce::Colours::whitesmoke);
    auto tempArea = juce::Rectangle<int>(getWidth() - 200, 2, 200 - 2, getHeight() - 4);
    g.drawRect(tempArea, 2);
    std::ostringstream oss;
    oss << std::fixed << std::setprecision(2) << timeBarPosition;
    g.drawText(oss.str() + "s", tempArea, juce::Justification::centred);
}

void MainToolbar::valueTreePropertyChanged(juce::ValueTree& treeWhosePropertyHasChanged,
                                           const juce::Identifier& property)
{
    // TODO: FIX IT SO THAT THE HALF OF THE BOX MEASURES THE SECONDS!!!!!!!!!!!!!!!!!!!!!!
    if(property.toString() == "timeBarTime")
    {
        timeBarPosition = tree["timeBarTime"];
        repaint();
    }
}