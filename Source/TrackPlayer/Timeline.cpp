#include "Timeline.h"

void Timeline::paint(juce::Graphics& g)
{
    g.setColour(juce::Colours::lightgrey);
    g.drawRect(getLocalBounds());
}

void Timeline::resized() {}