#include "TrackPlayer.h"

TrackPlayer::TrackPlayer()
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
    clipsBoxesComponent.setSize(timeline.getWidth(),
                                TrackPlayerConstants::startNumOfBoxesRows * TrackPlayerConstants::startBoxHeight);
    trackPlayerSideMenu.setBounds(
        0, 0, TrackPlayerConstants::trackPlayerSideMenuWidthRatio * getWidth(), clipsBoxesComponent.getHeight());

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

void TrackPlayer::drawBoxes()
{
    clipsBoxesVector.clear();
    for(auto i{0u}; i < TrackPlayerConstants::startNumOfBoxesRows; i++)
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
    for(auto i{0u}; i < TrackPlayerConstants::startNumOfBoxesRows + 1; i++)
    {
        auto recordButton = std::make_unique<juce::TextButton>("R");
        auto soloButton = std::make_unique<juce::TextButton>("S");
        auto muteButton = std::make_unique<juce::TextButton>("M");

        float currentY{/*timeline.getHeight() + */ i * TrackPlayerConstants::startBoxHeight + 15};

        recordButton->setBounds(10, currentY, trackButtonsSize, trackButtonsSize);
        recordButton->onClick = [i]() { std::cout << "Recording[" << i << "]" << std::endl; };

        soloButton->setBounds(45, currentY, trackButtonsSize, trackButtonsSize);
        soloButton->onClick = [i]() { std::cout << "Soloing[" << i << "]" << std::endl; };

        muteButton->setBounds(80, currentY, trackButtonsSize, trackButtonsSize);
        muteButton->onClick = [i]() { std::cout << "Muting[" << i << "]" << std::endl; };

        trackButtonsVector.push_back({std::move(recordButton), std::move(soloButton), std::move(muteButton)});
        for(auto& button: trackButtonsVector.back()) { trackPlayerSideMenu.addAndMakeVisible(button.get()); }
    }
}

void TrackPlayer::viewportsInit()
{
    trackPlayerViewport.setScrollBarsShown(true, true);
    trackPlayerViewport.setViewedComponent(&clipsBoxesComponent, false);
    // Temporary. Question: Why stepY = 26 scrolls whole box Height which is 85?
    trackPlayerViewport.setSingleStepSizes(8, 26);

    timelineViewport.setScrollBarsShown(false, false);
    timelineViewport.setViewedComponent(&timeline, false);

    trackPlayerSideMenuViewport.setScrollBarsShown(false, false);
    trackPlayerSideMenuViewport.setViewedComponent(&trackPlayerSideMenu, false);
}
