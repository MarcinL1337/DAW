#include "TrackPlayer.h"

TrackPlayer::TrackPlayer()
{
    addKeyListener(this);
    setWantsKeyboardFocus(true);
    addAndMakeVisible(trackPlayerViewport);
    addAndMakeVisible(timelineViewport);
    addAndMakeVisible(trackPlayerSideMenuViewport);
    addAndMakeVisible(timeBar);
    viewportsInit();
    setInterceptsMouseClicks(true, true);
    juce::Timer::callAfterDelay(100, [&] { grabKeyboardFocus(); });
}

void TrackPlayer::paint(juce::Graphics& g)
{
    g.setColour(juce::Colours::lightgrey);
    g.drawRect(getLocalBounds());
    g.setColour(juce::Colours::forestgreen);
    g.drawLine(trackPlayerSideMenu.getWidth() + timeBarXOffset + 7.5,
               timeline.getHeight(),
               trackPlayerSideMenu.getWidth() + timeBarXOffset + 7.5,
               std::max(getHeight(), timeline.getHeight() + clipsBoxesComponent.getHeight()));
    timelineViewport.setViewPosition(trackPlayerViewport.getViewPositionX(), timelineViewport.getViewPositionY());
    trackPlayerSideMenuViewport.setViewPosition(trackPlayerSideMenuViewport.getViewPositionX(),
                                                trackPlayerViewport.getViewPositionY());
    drawBoxes();
    drawTrackButtons(g);
}

void TrackPlayer::resized()
{
    timeline.setSize(TrackPlayerConstants::startNumOfBoxes * TrackPlayerConstants::startBoxWidth,
                     TrackPlayerConstants::timelineHeightRatio * getHeight());
    clipsBoxesComponent.setSize(timeline.getWidth(), getCurrentNumberOfTracks() * TrackPlayerConstants::startBoxHeight);
    trackPlayerSideMenu.setSize(TrackPlayerConstants::trackPlayerSideMenuWidthRatio * getWidth(),
                                clipsBoxesComponent.getHeight() + timeline.getHeight());
    timeBar.setBounds(trackPlayerSideMenu.getWidth() + timeBarXOffset, timeline.getHeight() - 15, 15, 15);

    timelineViewport.setBounds(getLocalBounds()
                                   .removeFromRight(getWidth() - trackPlayerSideMenu.getWidth())
                                   .removeFromTop(timeline.getHeight()));
    trackPlayerViewport.setBounds(getLocalBounds()
                                      .removeFromRight(getWidth() - trackPlayerSideMenu.getWidth())
                                      .removeFromBottom(getHeight() - timeline.getHeight()));
    trackPlayerSideMenuViewport.setBounds(0, timeline.getHeight(), trackPlayerSideMenu.getWidth(), getHeight());
    repaint();
}

void TrackPlayer::mouseDown(const juce::MouseEvent& event)
{
    juce::Point mouseClickPosition{event.getMouseDownPosition()};
    if(reallyContains(mouseClickPosition, true) and event.mods.isRightButtonDown())
    {
        addTrack();
    }

    lastMousePosition = event.getPosition();
    if(timelineViewport.reallyContains(lastMousePosition, true) and event.mods.isLeftButtonDown())
    {
        isCurrentlyDraggingTimeBar = true;
    }
}

void TrackPlayer::mouseUp(const juce::MouseEvent& event) { isCurrentlyDraggingTimeBar = false; }

void TrackPlayer::mouseDrag(const juce::MouseEvent& event)
{
    if(isCurrentlyDraggingTimeBar)
    {
        timeBarXOffset = juce::jlimit(0, getWidth(), timeBarXOffset + event.getPosition().x - lastMousePosition.x);
        lastMousePosition = event.getPosition();
        resized();
    }
}

bool TrackPlayer::keyPressed(const juce::KeyPress& key, Component* originatingComponent)
{
    if(key.getModifiers().isShiftDown() and key.getTextCharacter() == '+')
    {
        addTrack();
        return true;
    }
    return false;
}

void TrackPlayer::drawBoxes()
{
    clipsBoxesVector.clear();
    for(auto i{0u}; i < getCurrentNumberOfTracks(); i++)
    {
        auto clipsBox = std::make_unique<ClipsBox>(TrackPlayerConstants::startNumOfBoxes);
        clipsBox->setBounds(0,
                            i * TrackPlayerConstants::startBoxHeight,
                            clipsBoxesComponent.getWidth(),
                            TrackPlayerConstants::startBoxHeight);
        clipsBoxesVector.push_back(std::move(clipsBox));
        clipsBoxesComponent.addAndMakeVisible(clipsBoxesVector.back().get());
    }
}

void TrackPlayer::drawTrackButtons(juce::Graphics& g)
{
    trackButtonsVector.clear();
    const auto startX{trackPlayerSideMenu.getWidth() - 2 * trackButtonsSize};
    const auto xDifference{trackButtonsSize + 5};
    for(auto i{0u}; i < getCurrentNumberOfTracks(); i++)
    {
        auto recordButton = std::make_unique<juce::TextButton>("R");
        auto soloButton = std::make_unique<juce::TextButton>("S");
        auto muteButton = std::make_unique<juce::TextButton>("M");

        auto currentY{i * TrackPlayerConstants::startBoxHeight + 15};

        recordButton->setBounds(startX, currentY, trackButtonsSize, trackButtonsSize);
        recordButton->onClick = [i]() { std::cout << "Recording[" << i << "]" << std::endl; };

        soloButton->setBounds(startX - xDifference, currentY, trackButtonsSize, trackButtonsSize);
        soloButton->onClick = [i]() { std::cout << "Soloing[" << i << "]" << std::endl; };

        muteButton->setBounds(startX - 2 * xDifference, currentY, trackButtonsSize, trackButtonsSize);
        muteButton->onClick = [i]() { std::cout << "Muting[" << i << "]" << std::endl; };

        // TODO: to be changed, it's only as a placeholder to differentiate rows
        g.setColour(juce::Colours::white);
        g.drawText("Track nr " + std::to_string(i + 1),
                   10,
                   currentY + timeline.getHeight(),
                   100,
                   trackButtonsSize,
                   juce::Justification::centred,
                   false);

        trackButtonsVector.push_back({std::move(recordButton), std::move(soloButton), std::move(muteButton)});
        for(auto& button: trackButtonsVector.back()) { trackPlayerSideMenu.addAndMakeVisible(button.get()); }
    }
}

void TrackPlayer::viewportsInit()
{
    // TODO: scrollbars width/height messes up scrolling on the edges (max to the right or max to down)
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
    incrementCurrentNumberOfTracks();
    trackPlayerSideMenu.incrementCurrentNumberOfTracks();
    assert(trackPlayerSideMenu.getCurrentNumberOfTracks() == getCurrentNumberOfTracks());
    resized();
}

