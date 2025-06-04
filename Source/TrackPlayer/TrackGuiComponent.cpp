#include "TrackGuiComponent.h"
#include "TrackGuiManager.h"

TrackGuiComponent::TrackGuiComponent(const juce::ValueTree& parentTree) : tree{parentTree} { tree.addListener(this); }

void TrackGuiComponent::paint(juce::Graphics& g)
{
    g.setColour(juce::Colours::forestgreen);
    timeBarPosition = timeBarTime * currentTrackGuiBoxWidth;
    g.drawLine(timeBarPosition, 0, timeBarPosition, getHeight());
}

void TrackGuiComponent::resized() {}

void TrackGuiComponent::valueTreePropertyChanged(juce::ValueTree&, const juce::Identifier& property)
{
    if(static_cast<int>(tree[property]) == ValueTreeConstants::doNothing)
        return;
    if(property == ValueTreeIDs::timeBarTime)
    {
        timeBarTime = tree[ValueTreeIDs::timeBarTime];
        repaint();
    }
}

void TrackGuiComponent::changeBoxWidth(const uint16_t newBoxWidth) { currentTrackGuiBoxWidth = newBoxWidth; }
