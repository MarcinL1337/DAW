#include "Timeline.h"

#include "TrackPlayer.h"

Timeline::Timeline(const int numOfBoxes) : tempNumOfSeconds{numOfBoxes} {}

void Timeline::paint(juce::Graphics& g)
{
    g.setColour(juce::Colours::lightgrey);

    // TODO: make this painting cleaner
    for(auto i{0u}; i <= tempNumOfSeconds; i++)
    {
        g.drawText(std::to_string(i),
                   i * TrackPlayerConstants::startBoxWidth + 3,
                   getHeight() - 20,
                   TrackPlayerConstants::startBoxWidth,
                   15,
                   juce::Justification::left);
        if(i % 5 == 0)
        {
            g.drawLine(TrackPlayerConstants::startBoxWidth * i,
                       getHeight(),
                       TrackPlayerConstants::startBoxWidth * i,
                       getHeight() * 0.5,
                       1);
        }
        else
        {
            g.drawLine(TrackPlayerConstants::startBoxWidth * i,
                       getHeight(),
                       TrackPlayerConstants::startBoxWidth * i,
                       getHeight() * 0.66,
                       0.75);
        }
    }
}

void Timeline::resized() {}
