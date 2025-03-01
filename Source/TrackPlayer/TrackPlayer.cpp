#include "TrackPlayer.h"

TrackPlayer::TrackPlayer()
{
    addKeyListener(this);
    setWantsKeyboardFocus(true);
    addAndMakeVisible(trackPlayerViewport);
    addAndMakeVisible(timelineViewport);
    addAndMakeVisible(trackPlayerSideMenuViewport);
    addAndMakeVisible(timeBar);
    addMouseListener(&timeBar, false);
    viewportsInit();
    // timeBar.setAlwaysOnTop(true);
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
    timeline.setSize(TrackPlayerConstants::startNumOfBoxes * TrackPlayerConstants::startBoxWidth,
                     TrackPlayerConstants::timelineHeightRatio * getHeight());
    clipsBoxesComponent.setSize(timeline.getWidth(), getCurrentNumberOfTracks() * TrackPlayerConstants::startBoxHeight);
    trackPlayerSideMenu.setSize(TrackPlayerConstants::trackPlayerSideMenuWidthRatio * getWidth(),
                                clipsBoxesComponent.getHeight() + timeline.getHeight());
    timeBar.setBounds(trackPlayerSideMenu.getWidth() + 10, 0, 2, getHeight());

    drawBoxes();
    drawTrackButtons();

    timelineViewport.setBounds(getLocalBounds()
                                   .removeFromRight(getWidth() - trackPlayerSideMenu.getWidth())
                                   .removeFromTop(timeline.getHeight()));
    trackPlayerViewport.setBounds(getLocalBounds()
                                      .removeFromRight(getWidth() - trackPlayerSideMenu.getWidth())
                                      .removeFromBottom(getHeight() - timeline.getHeight()));
    trackPlayerSideMenuViewport.setBounds(0, timeline.getHeight(), trackPlayerSideMenu.getWidth(), getHeight());
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

void TrackPlayer::drawTrackButtons()
{
    trackButtonsVector.clear();
    const auto startX{trackPlayerSideMenu.getWidth() - 2 * trackButtonsSize};
    const auto xDifference{trackButtonsSize + 5};
    for(auto i{0u}; i < getCurrentNumberOfTracks(); i++)
    {
        auto recordButton = std::make_unique<juce::TextButton>("R");
        auto soloButton = std::make_unique<juce::TextButton>("S");
        auto muteButton = std::make_unique<juce::TextButton>("M");
        // TODO (1): trackLabel text get blurry after adding track rows/resizing (why?)
        // TODO (2): Labels makes painting MUCH slower, as it turns out, rendering text is expensive.
        auto trackLabel = std::make_unique<juce::Label>();

        auto currentY{i * TrackPlayerConstants::startBoxHeight + 15};

        recordButton->setBounds(startX, currentY, trackButtonsSize, trackButtonsSize);
        recordButton->onClick = [i]() { std::cout << "Recording[" << i << "]" << std::endl; };

        soloButton->setBounds(startX - xDifference, currentY, trackButtonsSize, trackButtonsSize);
        soloButton->onClick = [i]() { std::cout << "Soloing[" << i << "]" << std::endl; };

        muteButton->setBounds(startX - 2 * xDifference, currentY, trackButtonsSize, trackButtonsSize);
        muteButton->onClick = [i]() { std::cout << "Muting[" << i << "]" << std::endl; };

        trackLabel->setText("Track nr " + std::to_string(i + 1), juce::NotificationType::dontSendNotification);
        trackLabel->setBounds(30, currentY, 100, trackButtonsSize);

        trackButtonsVector.push_back({std::move(recordButton), std::move(soloButton), std::move(muteButton)});
        for(auto& button: trackButtonsVector.back()) { trackPlayerSideMenu.addAndMakeVisible(button.get()); }

        trackLabelsVector.push_back(std::move(trackLabel));
        trackPlayerSideMenu.addAndMakeVisible(trackLabelsVector.back().get());
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
    this->resized();
}

