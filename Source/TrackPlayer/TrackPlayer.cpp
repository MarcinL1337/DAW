#include "TrackPlayer.h"

#include "../../cmake-build-release-visual-studio/_deps/juce-src/modules/juce_graphics/fonts/harfbuzz/hb-aat-layout-trak-table.hh"

TrackPlayer::TrackPlayer()
{
    addAndMakeVisible(trackPlayerViewport);
    addAndMakeVisible(timelineViewport);
    addAndMakeVisible(trackPlayerSideMenu);
    viewportsInit();
}

void TrackPlayer::paint(juce::Graphics& g)
{
    g.setColour(juce::Colours::lightgrey);
    g.drawRect(getLocalBounds());
    timelineViewport.setViewPosition(trackPlayerViewport.getViewPositionX(), timelineViewport.getViewPositionY());
}

void TrackPlayer::resized()
{
    trackPlayerSideMenu.setBounds(0, 0, TrackPlayerConstants::trackPlayerSideMenuWidthRatio * getWidth(), getHeight());
    timeline.setSize(TrackPlayerConstants::startNumOfBoxes * TrackPlayerConstants::startBoxWidth,
                     TrackPlayerConstants::timelineHeightRatio * getHeight());
    clipsBoxesComponent.setSize(timeline.getWidth(),
                                TrackPlayerConstants::startNumOfBoxesRows * TrackPlayerConstants::startBoxHeight);

    // for(auto& box: clipsBoxesVector) { box.get()->setBounds() }
    drawBoxes();

    timelineViewport.setBounds(getLocalBounds()
                                   .removeFromRight(getWidth() - trackPlayerSideMenu.getWidth())
                                   .removeFromTop(timeline.getHeight()));
    trackPlayerViewport.setBounds(getLocalBounds()
                                      .removeFromRight(getWidth() - trackPlayerSideMenu.getWidth())
                                      .removeFromBottom(getHeight() - timeline.getHeight()));

    std::cerr << "resized\n";
    std::cerr << getLocalBounds().toString() << std::endl;
    std::cerr << getBounds().toString() << std::endl;
    std::cerr << "timeline: " << timeline.getBounds().toString() << std::endl;
    std::cerr << "trackPlayerSideMenu: " << trackPlayerSideMenu.getBounds().toString() << std::endl;
    std::cerr << "clipsBoxesComponent: " << clipsBoxesComponent.getBounds().toString() << std::endl;
}

void TrackPlayer::drawBoxes()
{
    clipsBoxesVector.clear();
    for(auto i{0u}; i < TrackPlayerConstants::startNumOfBoxesRows; i++)
    {
        int x{trackPlayerViewport.getX()};
        int y{trackPlayerViewport.getY()};
        std::cerr << "drawBoxes: (" << x << ", " << y << ")" << std::endl;
        auto clipsBox = std::make_unique<ClipsBox>(TrackPlayerConstants::startNumOfBoxes);
        clipsBox->setBounds(x,
                            y + (i * TrackPlayerConstants::startBoxHeight),
                            clipsBoxesComponent.getWidth(),
                            TrackPlayerConstants::startBoxHeight);
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
