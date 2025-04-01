#include "ClipsBoxesComponent.h"

ClipsBoxesComponent::ClipsBoxesComponent(const juce::ValueTree& parentTree) : tree{parentTree}
{
    tree.addListener(this);
}

void ClipsBoxesComponent::paint(juce::Graphics& g)
{
    g.setColour(juce::Colours::forestgreen);
    float timeBarPosition = timeBarTime * TrackPlayerConstants::startBoxWidth;
    g.drawLine(timeBarPosition, 0, timeBarPosition, getHeight());
}

void ClipsBoxesComponent::resized() {}

void ClipsBoxesComponent::valueTreePropertyChanged(juce::ValueTree& treeWhosePropertyHasChanged,
                                                   const juce::Identifier& property)
{
    if(property.toString() == "timeBarTime")
    {
        timeBarTime = tree["timeBarTime"];
        repaint();
    }
}
