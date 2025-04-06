#include "Timeline.h"

#include "TrackPlayer.h"

Timeline::Timeline(const int numOfBoxes, const juce::ValueTree& parentTree) : tree{parentTree}, numOfSeconds{numOfBoxes}
{
    addAndMakeVisible(timeBar);
}

void Timeline::paint(juce::Graphics& g)
{
    g.setColour(juce::Colours::lightgrey);

    for(auto i{0u}; i <= numOfSeconds; i++)
    {
        g.drawText(std::to_string(i),
                   i * TrackPlayerConstants::startBoxWidth + 3,
                   getHeight() - 20,
                   TrackPlayerConstants::startBoxWidth,
                   15,
                   juce::Justification::left);

        drawLineOnTimeline(g, i);
    }
}

void Timeline::drawLineOnTimeline(juce::Graphics& g, const uint32_t lineNumber) const
{
    auto heightScale = lineNumber % 5 ? 0.66f : 0.5f;
    g.drawLine(TrackPlayerConstants::startBoxWidth * lineNumber,
               getHeight(),
               TrackPlayerConstants::startBoxWidth * lineNumber,
               getHeight() * heightScale,
               0.75);
}

void Timeline::resized()
{
    timeBar.setBounds(timeBarXOffset,
                      getHeight() - TrackPlayerConstants::timeBarBoxSize,
                      TrackPlayerConstants::timeBarBoxSize,
                      TrackPlayerConstants::timeBarBoxSize);

    timeBarTimeInSeconds = (static_cast<float>(timeBarXOffset) + TrackPlayerConstants::timeBarBoxSize / 2.0f) /
                           getWidth() * TrackPlayerConstants::startNumOfBoxes;
    tree.setProperty(timeBarTime, timeBarTimeInSeconds, nullptr);
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
