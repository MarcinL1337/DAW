#include "TimeBar.h"

TimeBar::TimeBar() {}

void TimeBar::paint(juce::Graphics& g)
{
    g.setColour(juce::Colours::forestgreen);
    g.drawLine(0, 0, 0, getHeight(), 4);
}

void TimeBar::resized() {}