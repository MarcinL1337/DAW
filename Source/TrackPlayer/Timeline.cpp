#include "Timeline.h"

#include "TrackGuiManager.h"

Timeline::Timeline(const int numOfSeconds, const juce::ValueTree& parentTree) :
    tree{parentTree}, currentNumOfSeconds{numOfSeconds}
{
    addAndMakeVisible(timeBar);
    tree.addListener(this);
    tree.setProperty(timeBarTime, timeBarTimeInSeconds, nullptr);
}

void Timeline::paint(juce::Graphics& g)
{
    g.setColour(juce::Colours::lightgrey);
    for(auto i{0}; i <= currentNumOfSeconds; i++)
    {
        g.drawText(std::to_string(i),
                   i * currentTrackGuiBoxWidth + 3,
                   getHeight() - 20,
                   currentTrackGuiBoxWidth,
                   15,
                   juce::Justification::left);

        drawLineOnTimeline(g, i);
    }
}

void Timeline::drawLineOnTimeline(juce::Graphics& g, const uint32_t lineNumber) const
{
    auto heightScale = lineNumber % 5 ? 0.66f : 0.5f;
    g.drawLine(currentTrackGuiBoxWidth * lineNumber,
               getHeight(),
               currentTrackGuiBoxWidth * lineNumber,
               getHeight() * heightScale,
               0.75);
}

void Timeline::resized()
{
    timeBarXOffset =
        (timeBarTimeInSeconds * getWidth() / currentNumOfSeconds) - (TrackPlayerConstants::timeBarBoxSize / 2.0f);
    timeBar.setBounds(timeBarXOffset,
                      getHeight() - TrackPlayerConstants::timeBarBoxSize,
                      TrackPlayerConstants::timeBarBoxSize,
                      TrackPlayerConstants::timeBarBoxSize);
}

void Timeline::mouseDown(const juce::MouseEvent& event)
{
    lastMousePosition = event.getPosition();
    if(timeBar.getBounds().contains(lastMousePosition) and event.mods.isLeftButtonDown())
    {
        isCurrentlyDraggingTimeBar = true;
        tree.setProperty(ValueTreeIDs::isCurrentlyDraggingTimeBar, true, nullptr);
    }
}

void Timeline::mouseUp(const juce::MouseEvent& event)
{
    if(isCurrentlyDraggingTimeBar)
    {
        tree.setProperty(ValueTreeIDs::setPlayheadPosition, ValueTreeConstants::doNothing, nullptr);
        tree.setProperty(ValueTreeIDs::setPlayheadPosition, timeBarTimeInSeconds, nullptr);
        isCurrentlyDraggingTimeBar = false;
        tree.setProperty(ValueTreeIDs::isCurrentlyDraggingTimeBar, false, nullptr);
    }
}

void Timeline::mouseDrag(const juce::MouseEvent& event)
{
    // TODO: add screen moving right/left when dragging to borders
    if(isCurrentlyDraggingTimeBar)
    {
        timeBarXOffset = juce::jlimit(-TrackPlayerConstants::timeBarBoxSize / 2.0f,
                                      static_cast<float>(getWidth()),
                                      static_cast<float>(timeBarXOffset + event.getPosition().x - lastMousePosition.x));
        timeBarTimeInSeconds = (static_cast<float>(timeBarXOffset) + TrackPlayerConstants::timeBarBoxSize / 2.0f) /
                               getWidth() * currentNumOfSeconds;
        tree.setProperty(ValueTreeIDs::timeBarTime, timeBarTimeInSeconds, nullptr);
        lastMousePosition = event.getPosition();
        resized();
    }
}

void Timeline::valueTreePropertyChanged(juce::ValueTree&, const juce::Identifier& property)
{
    if(static_cast<int>(tree[property]) == ValueTreeConstants::doNothing)
        return;
    if(property == ValueTreeIDs::timeBarTime && !isCurrentlyDraggingTimeBar)
    {
        timeBarTimeInSeconds = tree[timeBarTime];
        resized();
    }
}