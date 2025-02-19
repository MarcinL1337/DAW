#include "TrackPlayer.h"

TrackPlayer::TrackPlayer()
{
    addAndMakeVisible(trackPlayerViewport);
    addAndMakeVisible(timelineViewport);
    addAndMakeVisible(trackPlayerSideMenu);
    viewportsInit();
    flexBoxInit();
    drawBoxes();
}

void TrackPlayer::paint(juce::Graphics& g)
{
    g.setColour(juce::Colours::lightgrey);
    g.drawRect(getLocalBounds());
    timelineViewport.setViewPosition(trackPlayerViewport.getViewPositionX(), timelineViewport.getViewPositionY());
}

void TrackPlayer::resized()
{
    // trackPlayerFlexBox.performLayout(getLocalBounds());
    trackPlayerWrapperFlexBox.performLayout(getLocalBounds());

    clipsBoxesComponent.setSize(
        TrackPlayerConstants::startNumOfBoxes * TrackPlayerConstants::startBoxWidth + trackPlayerSideMenu.getWidth(),
        TrackPlayerConstants::startNumOfBoxesRows * TrackPlayerConstants::startBoxHeight);
    timeline.setSize(clipsBoxesComponent.getWidth() + trackPlayerSideMenu.getWidth() + 10 /* temporary */,
                     timeline.getHeight());
    trackPlayerViewport.setBounds(
        getX(), clipsBoxesComponent.getY() + timeline.getHeight(), getWidth(), getHeight() - timeline.getHeight());
    timelineViewport.setBounds(getX(), timeline.getY(), getWidth(), timeline.getHeight());
    for(auto i{0u}; i < TrackPlayerConstants::startNumOfBoxesRows; i++)
    {
        clipsBoxesVector.at(i)->setBounds(0,
                                          i * clipsBoxesVector.at(i)->getGridBoxHeight() + clipsBoxesComponent.getY(),
                                          getWidth() * 2,
                                          getHeight() * 2);
    }
}

void TrackPlayer::flexBoxInit()
{
    trackPlayerWrapperFlexBox.flexDirection = juce::FlexBox::Direction::row;

    trackPlayerFlexBox.flexDirection = juce::FlexBox::Direction::column;
    trackPlayerFlexBox.flexWrap = juce::FlexBox::Wrap::noWrap;

    clipsBoxesFlexBox.flexDirection = juce::FlexBox::Direction::column;
    clipsBoxesFlexBox.flexWrap = juce::FlexBox::Wrap::noWrap;

    trackPlayerWrapperFlexBox.items.add(
        juce::FlexItem(trackPlayerSideMenu)
            .withFlex(0, 1, TrackPlayerConstants::trackPlayerSideMenuWidthRatio * getParentWidth())
            .withMinWidth(TrackPlayerConstants::minTrackPlayerSideMenuWidthRatio));
    trackPlayerWrapperFlexBox.items.add(juce::FlexItem(trackPlayerFlexBox).withFlex(1));

    trackPlayerFlexBox.items.add(juce::FlexItem(timeline)
                                     .withFlex(0, 1, TrackPlayerConstants::timelineHeightRatio * getParentHeight())
                                     .withMinHeight(TrackPlayerConstants::minTimelineHeightRatio * getParentHeight()));
    trackPlayerFlexBox.items.add(juce::FlexItem(clipsBoxesFlexBox).withFlex(1));

    clipsBoxesFlexBox.items.add(juce::FlexItem(clipsBoxesComponent));
}

void TrackPlayer::drawBoxes()
{
    clipsBoxesVector.clear();
    for(auto i{0u}; i < TrackPlayerConstants::startNumOfBoxesRows; i++)
    {
        float x{0.0f};
        float y{static_cast<float>(clipsBoxesComponent.getY())};
        auto clipsBox = std::make_unique<ClipsBox>(x, y, TrackPlayerConstants::startNumOfBoxes);
        clipsBoxesVector.push_back(std::move(clipsBox));
        clipsBoxesComponent.addAndMakeVisible(clipsBoxesVector.back().get());
    }
}

void TrackPlayer::viewportsInit()
{
    trackPlayerViewport.setScrollBarsShown(true, true);
    trackPlayerViewport.setViewedComponent(&clipsBoxesComponent, false);

    timelineViewport.setScrollBarsShown(false, false);
    timelineViewport.setViewedComponent(&timeline, false);
}
