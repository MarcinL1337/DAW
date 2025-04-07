#include "TrackPlayer.h"

TrackPlayer::TrackPlayer(const juce::ValueTree& parentTree) :
    tree{parentTree}, timeline{TrackPlayerConstants::startNumOfBoxes, parentTree}, clipsBoxesComponent{parentTree}
{
    addKeyListener(this);
    setWantsKeyboardFocus(true);
    addAndMakeVisible(trackPlayerViewport);
    addAndMakeVisible(timelineViewport);
    addAndMakeVisible(trackPlayerSideMenuViewport);
    viewportsInit();
    juce::Timer::callAfterDelay(100, [&] { grabKeyboardFocus(); });
    juce::Timer::callAfterDelay(50, [&] { addTrack(); });
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

    clipsBoxesComponent.setSize(timeline.getWidth(), sideMenuHeight - timeline.getHeight());
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

void TrackPlayer::mouseDown(const juce::MouseEvent& event)
{
    juce::Point mouseClickPosition{event.getMouseDownPosition()};
    if(reallyContains(mouseClickPosition, true) and event.mods.isRightButtonDown())
    {
        addTrack();
    }
}

bool TrackPlayer::keyPressed(const juce::KeyPress& key, Component* originatingComponent)
{
    if(key.getModifiers().isShiftDown() and key.getTextCharacter() == '+')
    {
        addTrack();
        return true;
    }
    if(key.getModifiers().isShiftDown() and key.getTextCharacter() == '_')
    {
        removeTrack();
        return true;
    }
    return false;
}

void TrackPlayer::makeNewClipsBox()
{
    auto clipsBox = std::make_unique<ClipsBox>(TrackPlayerConstants::startNumOfBoxes);
    clipsBox->setBounds(0,
                        getCurrentNumberOfTracks() * TrackPlayerConstants::startBoxHeight,
                        clipsBoxesComponent.getWidth(),
                        TrackPlayerConstants::startBoxHeight);
    clipsBoxesVector.push_back(std::move(clipsBox));
    clipsBoxesComponent.addAndMakeVisible(clipsBoxesVector.back().get());
}

void TrackPlayer::viewportsInit()
{
    trackPlayerViewport.setScrollBarsShown(true, true);
    trackPlayerViewport.setViewedComponent(&clipsBoxesComponent, false);
    // TODO: Temporary. Question: Why stepY = 26 scrolls whole box Height which is 85?
    trackPlayerViewport.setSingleStepSizes(8, 26);

    timelineViewport.setScrollBarsShown(false, false);
    timelineViewport.setViewedComponent(&timeline, false);

    trackPlayerSideMenuViewport.setScrollBarsShown(false, false);
    trackPlayerSideMenuViewport.setViewedComponent(&trackPlayerSideMenu, false);
}

void TrackPlayer::addTrack()
{
    makeNewClipsBox();
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
        clipsBoxesVector.pop_back();
        decrementCurrentNumberOfTracks();
        trackPlayerSideMenu.decrementCurrentNumberOfTracks();
        assert(trackPlayerSideMenu.getCurrentNumberOfTracks() == getCurrentNumberOfTracks());
        resized();
        trackPlayerSideMenu.resized();
    }
}
