#include "TrackPlayer.h"

void TrackPlayer::paint(juce::Graphics& g)
{
    g.setColour(juce::Colours::lightgrey);
    g.drawRect(getLocalBounds());
}

void TrackPlayer::resized() {}