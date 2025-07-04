#include "ClipSplitBar.h"

ClipSplitBar::ClipSplitBar() { setInterceptsMouseClicks(false, false); }

void ClipSplitBar::resized() {}

void ClipSplitBar::paint(juce::Graphics& g)
{
    g.setColour(juce::Colours::white);
    g.drawLine(mouseXPosition, 0, mouseXPosition, getHeight(), 1);
}
