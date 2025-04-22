#include "TrackPlayer.h"

TrackPlayer::TrackPlayer(const juce::ValueTree& parentTree, TrackManager& trackManagerRef) :
    tree{parentTree},
    timeline{TrackPlayerConstants::startNumOfBoxes, parentTree},
    trackPlayerSideMenu(trackManagerRef),
    trackGuiComponent{parentTree}
{
    addAndMakeVisible(trackPlayerViewport);
    addAndMakeVisible(timelineViewport);
    addAndMakeVisible(trackPlayerSideMenuViewport);
    viewportsInit();
}

void TrackPlayer::paint(juce::Graphics& g)
{
    g.setColour(juce::Colours::lightgrey);
    g.drawRect(getLocalBounds());
    timelineViewport.setViewPosition(trackPlayerViewport.getViewPositionX(), timelineViewport.getViewPositionY());
    trackPlayerSideMenuViewport.setViewPosition(trackPlayerSideMenuViewport.getViewPositionX(),
                                                trackPlayerViewport.getViewPositionY());
}

void TrackPlayer::resized()
{
    timeline.setSize(TrackPlayerConstants::startNumOfBoxes * TrackPlayerConstants::startBoxWidth,
                     TrackPlayerConstants::timelineHeightRatio * getHeight());

    auto sideMenuHeight{std::max(getCurrentNumberOfTracks() * TrackPlayerConstants::startBoxHeight,
                                 static_cast<float>(getHeight() - trackPlayerViewport.getScrollBarThickness()))};

    trackGuiComponent.setSize(timeline.getWidth(), sideMenuHeight - timeline.getHeight());
    trackPlayerSideMenu.setBounds(
        0, timeline.getHeight(), TrackPlayerConstants::trackPlayerSideMenuWidthRatio * getWidth(), sideMenuHeight);

    trackPlayerViewport.setBounds(getLocalBounds()
                                      .removeFromRight(getWidth() - trackPlayerSideMenu.getWidth())
                                      .removeFromBottom(getHeight() - timeline.getHeight()));
    timelineViewport.setBounds(getLocalBounds()
                                   .removeFromRight(getWidth() - trackPlayerSideMenu.getWidth())
                                   .removeFromTop(timeline.getHeight())
                                   .withWidth(trackPlayerViewport.getViewWidth()));
    trackPlayerSideMenuViewport.setBounds(0,
                                          timeline.getHeight(),
                                          trackPlayerSideMenu.getWidth(),
                                          getHeight() - trackPlayerViewport.getScrollBarThickness());
}

void TrackPlayer::makeNewTrackGui()
{
    auto trackGui = std::make_unique<TrackGui>(TrackPlayerConstants::startNumOfBoxes);
    trackGui->setBounds(0,
                        getCurrentNumberOfTracks() * TrackPlayerConstants::startBoxHeight,
                        trackGuiComponent.getWidth(),
                        TrackPlayerConstants::startBoxHeight);
    trackGuiVector.push_back(std::move(trackGui));
    trackGuiComponent.addAndMakeVisible(trackGuiVector.back().get());
}

void TrackPlayer::viewportsInit()
{
    trackPlayerViewport.setScrollBarsShown(true, true);
    trackPlayerViewport.setViewedComponent(&trackGuiComponent, false);
    // TODO: Temporary. Question: Why stepY = 26 scrolls whole box Height which is 85?
    trackPlayerViewport.setSingleStepSizes(8, 26);

    timelineViewport.setScrollBarsShown(false, false);
    timelineViewport.setViewedComponent(&timeline, false);

    trackPlayerSideMenuViewport.setScrollBarsShown(false, false);
    trackPlayerSideMenuViewport.setViewedComponent(&trackPlayerSideMenu, false);
}

void TrackPlayer::addTrack()
{
    makeNewTrackGui();
    incrementCurrentNumberOfTracks();
    trackPlayerSideMenu.incrementCurrentNumberOfTracks();
    assert(trackPlayerSideMenu.getCurrentNumberOfTracks() == getCurrentNumberOfTracks());
    resized();
    trackPlayerSideMenu.resized();
}

void TrackPlayer::removeTrack()
{
    if(getCurrentNumberOfTracks() > 0)
    {
        trackGuiVector.pop_back();
        decrementCurrentNumberOfTracks();
        trackPlayerSideMenu.decrementCurrentNumberOfTracks();
        assert(trackPlayerSideMenu.getCurrentNumberOfTracks() == getCurrentNumberOfTracks());
        resized();
        trackPlayerSideMenu.resized();
    }
}
