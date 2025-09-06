#include "TrackGuiManager.h"

#include <random>

TrackGuiManager::TrackGuiManager(juce::ValueTree& parentTree) :
    tree{parentTree},
    timeline{currentNumOfSeconds, parentTree},
    trackPlayerSideMenu{parentTree},
    trackGuiComponent{parentTree}
{
    setWantsKeyboardFocus(true);
    addAndMakeVisible(trackPlayerViewport);
    addAndMakeVisible(timelineViewport);
    addAndMakeVisible(trackPlayerSideMenuViewport);
    setMouseClickGrabsKeyboardFocus(true);
    viewportsInit();
    tree.addListener(this);
    juce::Timer::callAfterDelay(100, [&] { grabKeyboardFocus(); });
    trackPlayerSideMenu.setSize(TrackPlayerConstants::trackPlayerSideMenuWidthRatio * getParentWidth(), 0);
}

void TrackGuiManager::paint(juce::Graphics& g)
{
    g.setColour(juce::Colours::lightgrey);
    g.drawRect(getLocalBounds());
    timelineViewport.setViewPosition(trackPlayerViewport.getViewPositionX(), timelineViewport.getViewPositionY());
    trackPlayerSideMenuViewport.setViewPosition(trackPlayerSideMenuViewport.getViewPositionX(),
                                                trackPlayerViewport.getViewPositionY());
}

void TrackGuiManager::resized()
{
    timeline.setSize(currentNumOfSeconds * currentTrackGuiBoxWidth,
                     TrackPlayerConstants::timelineHeightRatio * getHeight());

    // TODO: magic constant -> to be changed
    const auto sideMenuHeight{std::max(getCurrentNumberOfTracks() * currentTrackGuiBoxHeight,
                                       getHeight() - trackPlayerViewport.getScrollBarThickness()) +
                              105};

    trackGuiComponent.setSize(timeline.getWidth(), sideMenuHeight - timeline.getHeight());

    trackPlayerSideMenu.setBounds(0, timeline.getHeight(), trackPlayerSideMenu.getWidth(), sideMenuHeight);

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

void TrackGuiManager::viewportsInit()
{
    trackPlayerViewport.setScrollBarsShown(true, true);
    trackPlayerViewport.setViewedComponent(&trackGuiComponent, false);
    trackPlayerViewport.setSingleStepSizes(8, 26);

    timelineViewport.setScrollBarsShown(false, false);
    timelineViewport.setViewedComponent(&timeline, false);

    trackPlayerSideMenuViewport.setScrollBarsShown(false, false);
    trackPlayerSideMenuViewport.setViewedComponent(&trackPlayerSideMenu, false);
}

void TrackGuiManager::makeNewTrackGui()
{
    auto trackGui = std::make_unique<TrackGui>(currentTrackGuiBoxWidth, currentNumOfSeconds, tree);

    trackGui->setBounds(0,
                        getCurrentNumberOfTracks() * currentTrackGuiBoxHeight,
                        trackGuiComponent.getWidth(),
                        currentTrackGuiBoxHeight);
    trackGuiVector.push_back(std::move(trackGui));
    trackGuiComponent.addAndMakeVisible(trackGuiVector.back().get());
}

void TrackGuiManager::addTrack()
{
    makeNewTrackGui();
    trackPlayerSideMenu.addTrackControls();
    assert(trackPlayerSideMenu.getCurrentNumberOfTracks() == getCurrentNumberOfTracks());
    resized();
    trackPlayerSideMenu.resized();
}

void TrackGuiManager::setTrackButtonStates(const int trackIndex, const bool muted, const bool soloed) const
{
    trackPlayerSideMenu.updateTrackButtonStates(trackIndex, muted, soloed);
}

void TrackGuiManager::removeTrack(const int trackIndex)
{
    assert(trackIndex >= 0 && trackIndex < static_cast<int>(trackGuiVector.size()));

    for(size_t i = trackIndex; i < trackGuiVector.size() - 1; ++i)
        trackGuiVector[i + 1]->setBounds(
            0, i * currentTrackGuiBoxHeight, trackGuiComponent.getWidth(), currentTrackGuiBoxHeight);

    trackPlayerSideMenu.removeTrackControls(trackIndex);
    trackGuiVector.erase(trackGuiVector.begin() + trackIndex);
    assert(trackPlayerSideMenu.getCurrentNumberOfTracks() == getCurrentNumberOfTracks());
    resized();
}

void TrackGuiManager::valueTreePropertyChanged(juce::ValueTree&, const juce::Identifier& property)
{
    if(static_cast<int>(tree[property]) == ValueTreeConstants::doNothing)
        return;
    if(property == ValueTreeIDs::numOfSecondsChanged)
    {
        const int newNumOfSeconds = tree[ValueTreeIDs::numOfSecondsChanged];
        if(newNumOfSeconds > currentNumOfSeconds)
        {
            currentNumOfSeconds = newNumOfSeconds;
            timeline.changeNumOfSeconds(currentNumOfSeconds);
            resized();
            for(const auto& trackGui: trackGuiVector)
            {
                trackGui->changeNumOfSeconds(currentNumOfSeconds);
                trackGui->setSize(trackGuiComponent.getWidth(), currentTrackGuiBoxHeight);
                trackGui->repaint();
            }
        }
    }
    else if(property == ValueTreeIDs::trackPlayerZoomPercentage)
    {
        const int newZoomPercentage{tree[ValueTreeIDs::trackPlayerZoomPercentage]};
        changeTrackGuiBoxWidthAndPropagate(newZoomPercentage);
    }
    else if(property == ValueTreeIDs::timeBarTime)
    {
        updatePlayheadFollowing();
    }
    else if(property == ValueTreeIDs::followModeChanged)
    {
        setFollowMode(
            static_cast<PlayheadFollowConstants::Mode>(static_cast<int>(tree[ValueTreeIDs::followModeChanged])));
    }
    else if(property == ValueTreeIDs::reorderTracks)
    {
        const int fromIndex = tree[ValueTreeIDs::reorderTracks][0];
        const int toIndex = tree[ValueTreeIDs::reorderTracks][1];
        reorderTrackGuis(fromIndex, toIndex);
    }
}

void TrackGuiManager::addWaveformToTrackGui(const juce::String& newAudioFilePath, const int trackIndex,
                                            const NodeID newAudioClipID) const
{
    assert(trackIndex >= 0 && trackIndex < static_cast<int>(trackGuiVector.size()));
    trackGuiVector[trackIndex]->addNewAudioFile(newAudioFilePath, newAudioClipID);
}

void TrackGuiManager::setOffsetOfWaveformInSeconds(const int trackIndex, const NodeID audioClipID,
                                                   const double offsetSeconds) const
{
    assert(trackIndex >= 0 && trackIndex < getCurrentNumberOfTracks());

    trackGuiVector[trackIndex]->setOffsetOfWaveformInSeconds(audioClipID, offsetSeconds);
}

void TrackGuiManager::changeTrackGuiBoxWidthAndPropagate(const int newBoxWidthPercentage)
{
    const auto currentViewWidth = trackPlayerViewport.getViewWidth();
    const auto centerTimePosition =
        static_cast<double>(trackPlayerViewport.getViewPositionX() + currentViewWidth / 2) / currentTrackGuiBoxWidth;

    currentTrackGuiBoxWidth = baseTrackGuiBoxWidth * newBoxWidthPercentage / 100;
    resized();
    timeline.changeBoxWidth(currentTrackGuiBoxWidth);
    trackGuiComponent.changeBoxWidth(currentTrackGuiBoxWidth);
    for(const auto& trackGui: trackGuiVector)
    {
        trackGui->setSize(trackGuiComponent.getWidth(), currentTrackGuiBoxHeight);
        trackGui->changeBoxWidth(currentTrackGuiBoxWidth);
    }

    const auto newViewX = static_cast<int>(centerTimePosition * currentTrackGuiBoxWidth - currentViewWidth / 2);
    trackPlayerViewport.setViewPosition(newViewX, trackPlayerViewport.getViewPositionY());
}

void TrackGuiManager::updatePlayheadFollowing()
{
    if(!tree.hasProperty(ValueTreeIDs::timeBarTime))
        return;

    if(tree.hasProperty(ValueTreeIDs::isCurrentlyDraggingTimeBar) &&
       static_cast<bool>(tree[ValueTreeIDs::isCurrentlyDraggingTimeBar]))
        return;

    const auto timeBarValue = static_cast<float>(tree[ValueTreeIDs::timeBarTime]);
    const auto timeBarPosition = timeBarValue * currentTrackGuiBoxWidth;
    const auto viewportWidth = trackPlayerViewport.getViewWidth();
    const auto viewportX = trackPlayerViewport.getViewPositionX();

    switch(followMode)
    {
        case PlayheadFollowConstants::Mode::NoFollow:
            break;

        case PlayheadFollowConstants::Mode::SmoothFollow:
            trackPlayerViewport.setViewPosition(static_cast<int>(timeBarPosition - viewportWidth / 2),
                                                trackPlayerViewport.getViewPositionY());
            break;

        case PlayheadFollowConstants::Mode::JumpFollow:
            // viewportWidth / 30 defines a margin near the right edge of the viewport.
            // This ensures the playhead triggers a viewport scroll slightly before it actually leaves the visible area,
            // providing a smoother user experience and preventing the playhead from sticking to the very edge.
            if(timeBarPosition < viewportX || timeBarPosition > viewportX + viewportWidth - viewportWidth / 30)
            {
                trackPlayerViewport.setViewPosition(static_cast<int>(timeBarPosition),
                                                    trackPlayerViewport.getViewPositionY());
            }
            break;
        default:
            std::unreachable();
    }
}

void TrackGuiManager::reorderTrackGuis(const int fromIndex, const int toIndex)
{
    auto trackToMove = std::move(trackGuiVector[fromIndex]);
    trackGuiVector.erase(trackGuiVector.begin() + fromIndex);
    trackGuiVector.insert(trackGuiVector.begin() + toIndex, std::move(trackToMove));

    for(size_t i = 0; i < trackGuiVector.size(); ++i)
        trackGuiVector[i]->setTopLeftPosition(0, i * currentTrackGuiBoxHeight);
}

void TrackGuiManager::clearAllTracks()
{
    trackGuiVector.clear();
    trackPlayerSideMenu.clearAllTracks();
}

void TrackGuiManager::setTrackName(const int trackIndex, const juce::String& name) const
{
    trackPlayerSideMenu.setTrackName(trackIndex, name);
}

void TrackGuiManager::mouseWheelMove(const juce::MouseEvent& event, const juce::MouseWheelDetails& wheel)
{
    if(!event.mods.isCtrlDown())
        return;

    const float currentZoom = tree[ValueTreeIDs::trackPlayerZoomPercentage];
    const float zoomStep = wheel.deltaY > 0 ? 10.0f : -10.0f;
    const float newZoom = juce::jlimit(static_cast<float>(TrackPlayerConstants::minZoom),
                                       static_cast<float>(TrackPlayerConstants::maxZoom),
                                       currentZoom + zoomStep);

    if(newZoom != currentZoom)
        tree.setProperty(ValueTreeIDs::trackPlayerZoomPercentage, static_cast<int>(newZoom), nullptr);
}
