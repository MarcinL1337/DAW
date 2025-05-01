#include "TrackPlayer.h"

TrackPlayer::TrackPlayer(juce::ValueTree& parentTree) :
    tree{parentTree},
    timeline{currentNumOfSeconds, parentTree},
    trackPlayerSideMenu(parentTree),
    trackGuiComponent{parentTree}
{
    setWantsKeyboardFocus(true);
    addAndMakeVisible(trackPlayerViewport);
    addAndMakeVisible(timelineViewport);
    addAndMakeVisible(trackPlayerSideMenuViewport);
    viewportsInit();
    tree.addListener(this);
    juce::Timer::callAfterDelay(100, [&] { grabKeyboardFocus(); });
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
    timeline.setSize(currentNumOfSeconds * currentTrackGuiBoxWidth,
                     TrackPlayerConstants::timelineHeightRatio * getHeight());

    // TODO: magic constant -> to be changed
    const auto sideMenuHeight{std::max(getCurrentNumberOfTracks() * currentTrackGuiBoxHeight,
                                       getHeight() - trackPlayerViewport.getScrollBarThickness()) +
                              105};

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

void TrackPlayer::makeNewTrackGui(const juce::String& newAudioFilePath)
{
    auto trackGui =
        newAudioFilePath.isEmpty()
            ? std::make_unique<TrackGui>(currentTrackGuiBoxWidth, currentNumOfSeconds, tree)
            : std::make_unique<TrackGui>(currentTrackGuiBoxWidth, currentNumOfSeconds, tree, newAudioFilePath);

    trackGui->setBounds(0,
                        getCurrentNumberOfTracks() * currentTrackGuiBoxHeight,
                        trackGuiComponent.getWidth(),
                        currentTrackGuiBoxHeight);
    trackGuiVector.push_back(std::move(trackGui));
    trackGuiComponent.addAndMakeVisible(trackGuiVector.back().get());
}

void TrackPlayer::addTrack(const juce::String& newAudioFilePath)
{
    newAudioFilePath.isEmpty() ? makeNewTrackGui() : makeNewTrackGui(newAudioFilePath);

    trackPlayerSideMenu.incrementCurrentNumberOfTracks();
    assert(trackPlayerSideMenu.getCurrentNumberOfTracks() == getCurrentNumberOfTracks());
    resized();
    trackPlayerSideMenu.resized();
}

void TrackPlayer::removeTrack(const int trackIndex)
{
    assert(trackIndex >= 0 && trackIndex < static_cast<int>(trackGuiVector.size()));
    trackGuiVector.erase(trackGuiVector.begin() + trackIndex);

    for(size_t i = trackIndex; i < trackGuiVector.size(); ++i)
        trackGuiVector[i]->setBounds(
            0, i * currentTrackGuiBoxHeight, trackGuiComponent.getWidth(), currentTrackGuiBoxHeight);

    trackPlayerSideMenu.decrementCurrentNumberOfTracks();
    assert(trackPlayerSideMenu.getCurrentNumberOfTracks() == getCurrentNumberOfTracks());
    resized();
    trackPlayerSideMenu.resized();
}

void TrackPlayer::valueTreePropertyChanged(juce::ValueTree&, const juce::Identifier& property)
{
    if(static_cast<int>(tree[property.toString()]) == ValueTreeConstants::doNothing)
        return;
    if(property.toString() == "numOfSecondsChanged")
    {
        currentNumOfSeconds = tree["numOfSecondsChanged"];
        timeline.changeNumOfSeconds(currentNumOfSeconds);
        resized();
        for(const auto& trackGui: trackGuiVector)
        {
            trackGui->changeNumOfSeconds(currentNumOfSeconds);
            trackGui->setSize(trackGuiComponent.getWidth(), currentTrackGuiBoxHeight);
            trackGui->repaint();
        }
    }
    else if(property.toString() == "trackPlayerZoomPercentage")
    {
        int newZoomPercentage{tree["trackPlayerZoomPercentage"]};
        changeTrackGuiBoxWidthAndPropagate(newZoomPercentage);
    }
}

TrackGui* TrackPlayer::findFirstEmptyTrackGui() const
{
    for(auto& trackGui: trackGuiVector)
        if(trackGui->hasNoAudioClips())
            return trackGui.get();
    return nullptr;
}

void TrackPlayer::addWaveformToTrackGui(const juce::String& newAudioFilePath, const int trackIndex)
{
    if(trackIndex >= 0 && trackIndex < static_cast<int>(trackGuiVector.size()))
    {
        trackGuiVector[trackIndex]->addNewAudioFile(newAudioFilePath);
    }
    else if(trackIndex == -1)
    {
        if(const auto maybeFreeTrackGui = findFirstEmptyTrackGui(); maybeFreeTrackGui != nullptr)
            maybeFreeTrackGui->addNewAudioFile(newAudioFilePath);
        else
            addTrack(newAudioFilePath);
    }
}

void TrackPlayer::changeTrackGuiBoxWidthAndPropagate(const int newBoxWidthPercentage)
{
    currentTrackGuiBoxWidth = baseTrackGuiBoxWidth * newBoxWidthPercentage / 100;
    std::cerr << "currentTrackGuiBoxWidth = " << currentTrackGuiBoxWidth << std::endl;
    resized();
    timeline.changeBoxWidth(currentTrackGuiBoxWidth);
    trackGuiComponent.changeBoxWidth(currentTrackGuiBoxWidth);
    for(const auto& trackGui: trackGuiVector)
    {
        trackGui->setSize(trackGuiComponent.getWidth(), currentTrackGuiBoxHeight);
        trackGui->changeBoxWidth(currentTrackGuiBoxWidth);
    }
}
