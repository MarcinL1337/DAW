#include "Timeline.h"

#include "TrackPlayer.h"

Timeline::Timeline(const int numOfBoxes, const juce::ValueTree& parentTree) :
    tree{parentTree}, tempNumOfSeconds{numOfBoxes}
{
    addAndMakeVisible(timeBar);
    tree.setProperty(timeBarTime, 0.0, nullptr);
}

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

void Timeline::resized()
{
    timeBar.setBounds(timeBarXOffset,
                      getHeight() - TrackPlayerConstants::timeBarBoxSize,
                      TrackPlayerConstants::timeBarBoxSize,
                      TrackPlayerConstants::timeBarBoxSize);
    tree.setProperty(timeBarTime, timeBarTimeInSeconds, nullptr);
    timeBarTimeInSeconds = (static_cast<float>(timeBarXOffset) + TrackPlayerConstants::timeBarBoxSize / 2.0f) /
                           getWidth() * TrackPlayerConstants::startNumOfBoxes;
}

void Timeline::mouseDown(const juce::MouseEvent& event)
{
    lastMousePosition = event.getPosition();
    if(timeBar.getBounds().contains(lastMousePosition) and event.mods.isLeftButtonDown())
    {
        isCurrentlyDraggingTimeBar = true;
    }
}

void Timeline::mouseUp(const juce::MouseEvent& event) { isCurrentlyDraggingTimeBar = false; }

void Timeline::mouseDrag(const juce::MouseEvent& event)
{
    if(isCurrentlyDraggingTimeBar)
    {
        timeBarXOffset = juce::jlimit(-TrackPlayerConstants::timeBarBoxSize / 2.0f,
                                      static_cast<float>(getWidth()),
                                      static_cast<float>(timeBarXOffset + event.getPosition().x - lastMousePosition.x));
        lastMousePosition = event.getPosition();
        resized();
    }
}
