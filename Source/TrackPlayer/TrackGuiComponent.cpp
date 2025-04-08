#include "TrackGuiComponent.h"

TrackGuiComponent::TrackGuiComponent(const juce::ValueTree& parentTree) : tree{parentTree}
{
    tree.addListener(this);
}

void TrackGuiComponent::paint(juce::Graphics& g)
{
    g.setColour(juce::Colours::forestgreen);
    timeBarPosition =
        timeBarTime == 0 ? TrackPlayerConstants::timeBarBoxSize / 2 : timeBarTime * TrackPlayerConstants::startBoxWidth;
    g.drawLine(timeBarPosition, 0, timeBarPosition, getHeight());
}

void TrackGuiComponent::resized() {}

void TrackGuiComponent::valueTreePropertyChanged(juce::ValueTree& treeWhosePropertyHasChanged,
                                                   const juce::Identifier& property)
{
    if(property.toString() == "timeBarTime")
    {
        timeBarTime = tree["timeBarTime"];
        repaint();
    }
}
