#include "TrackPlayer.h"

TrackPlayer::TrackPlayer(const juce::ValueTree& parentTree) :
    tree{parentTree}, timeline{TrackPlayerConstants::startNumOfBoxes, parentTree}
{
    addKeyListener(this);
    tree.addListener(this);
    setWantsKeyboardFocus(true);
    addAndMakeVisible(trackPlayerViewport);
    addAndMakeVisible(timelineViewport);
    addAndMakeVisible(trackPlayerSideMenuViewport);
    viewportsInit();
    juce::Timer::callAfterDelay(100, [&] { grabKeyboardFocus(); });
}

void TrackPlayer::paint(juce::Graphics& g)
{
    g.setColour(juce::Colours::lightgrey);
    g.drawRect(getLocalBounds());
    timelineViewport.setViewPosition(trackPlayerViewport.getViewPositionX(), timelineViewport.getViewPositionY());
    trackPlayerSideMenuViewport.setViewPosition(trackPlayerSideMenuViewport.getViewPositionX(),
                                                trackPlayerViewport.getViewPositionY());

    g.setColour(juce::Colours::forestgreen);
    float timeBarPosition = trackPlayerSideMenu.getWidth() + TrackPlayerConstants::timeBarBoxSize / 2 +
                            timeBarTime * TrackPlayerConstants::startBoxWidth;
    g.drawLine(
        timeBarPosition, timeline.getHeight(), timeBarPosition, std::max(getHeight(), clipsBoxesComponent.getHeight()));

    drawBoxes();
    drawTrackText(g);
}

void TrackPlayer::resized()
{
    timeline.setSize(TrackPlayerConstants::startNumOfBoxes * TrackPlayerConstants::startBoxWidth,
                     TrackPlayerConstants::timelineHeightRatio * getHeight());
    clipsBoxesComponent.setSize(timeline.getWidth(), getCurrentNumberOfTracks() * TrackPlayerConstants::startBoxHeight);
    trackPlayerSideMenu.setSize(TrackPlayerConstants::trackPlayerSideMenuWidthRatio * getWidth(),
                                clipsBoxesComponent.getHeight() + timeline.getHeight());

    timelineViewport.setBounds(getLocalBounds()
                                   .removeFromRight(getWidth() - trackPlayerSideMenu.getWidth())
                                   .removeFromTop(timeline.getHeight()));
    trackPlayerViewport.setBounds(getLocalBounds()
                                      .removeFromRight(getWidth() - trackPlayerSideMenu.getWidth())
                                      .removeFromBottom(getHeight() - timeline.getHeight()));
    trackPlayerSideMenuViewport.setBounds(0, timeline.getHeight(), trackPlayerSideMenu.getWidth(), getHeight());
    drawTrackButtons();
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

        auto currentY{i * TrackPlayerConstants::startBoxHeight + 15};

        recordButton->setBounds(startX, currentY, trackButtonsSize, trackButtonsSize);
        recordButton->onClick = [i]() { std::cout << "Recording[" << i + 1 << "]" << std::endl; };

        soloButton->setBounds(startX - xDifference, currentY, trackButtonsSize, trackButtonsSize);
        soloButton->onClick = [i]() { std::cout << "Soloing[" << i + 1 << "]" << std::endl; };

        muteButton->setBounds(startX - 2 * xDifference, currentY, trackButtonsSize, trackButtonsSize);
        muteButton->onClick = [i]() { std::cout << "Muting[" << i + 1 << "]" << std::endl; };

        trackButtonsVector.push_back({std::move(recordButton), std::move(soloButton), std::move(muteButton)});
        for(auto& button: trackButtonsVector.back()) { trackPlayerSideMenu.addAndMakeVisible(button.get()); }
    }
}

void TrackPlayer::drawTrackText(juce::Graphics& g) const
{
    for(auto i{0u}; i < getCurrentNumberOfTracks(); i++)
    {
        // TODO: to be changed, it's only as a placeholder to differentiate rows + the numbers of tracks don't scroll
        auto currentY{i * TrackPlayerConstants::startBoxHeight + 15};
        g.setColour(juce::Colours::white);
        g.drawText("Track nr " + std::to_string(i + 1),
                   10,
                   currentY + timeline.getHeight(),
                   100,
                   trackButtonsSize,
                   juce::Justification::centred,
                   false);
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

void TrackPlayer::valueTreePropertyChanged(juce::ValueTree& treeWhosePropertyHasChanged,
                                           const juce::Identifier& property)
{
    if(property.toString() == "timeBarTime")
    {
        timeBarTime = tree["timeBarTime"];
        repaint();
    }
}
