#include "TimeBar.h"

TimeBar::TimeBar() { setInterceptsMouseClicks(false, false); }

void TimeBar::paint(juce::Graphics& g)
{
    g.setColour(juce::Colours::forestgreen);
    setAlpha(0.6);
    g.fillRect(getLocalBounds());
}

void TimeBar::resized() {}
