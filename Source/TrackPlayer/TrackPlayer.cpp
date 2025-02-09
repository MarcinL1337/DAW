#include "TrackPlayer.h"

TrackPlayer::TrackPlayer()
{
    // addAndMakeVisible(timeline);
    addAndMakeVisible(trackPlayerViewport);
    addAndMakeVisible(timelineViewport);
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
    // std::cout << "clipsBoxesComponent: " << clipsBoxesComponent.getHeight() << " x " <<
    // clipsBoxesComponent.getWidth()
    //           << " | x = " << clipsBoxesComponent.getX() << ", y = " << clipsBoxesComponent.getY() << std::endl;
    // std::cout << "timeline: " << timeline.getHeight() << " x " << timeline.getWidth() << " | x = " << timeline.getX()
    //           << ", y = " << timeline.getY() << std::endl;
    trackPlayerFlexBox.performLayout(getLocalBounds());
    trackPlayerViewport.setBounds(clipsBoxesComponent.getX(),
                                  clipsBoxesComponent.getY() + timeline.getHeight(),
                                  getWidth(),
                                  getHeight() - timeline.getHeight());
    timelineViewport.setBounds(timeline.getX(), timeline.getY(), getWidth(), timeline.getHeight());
    clipsBoxesComponent.setSize(getWidth() * 2, getHeight() * 2);
    timeline.setSize(clipsBoxesComponent.getWidth(), timeline.getHeight());
    for(auto i{0}; i < 20; i++)
    {
        clipsBoxesVector.at(i)->setBounds(0,
                                          i * clipsBoxesVector.at(i)->getGridBoxHeight() + clipsBoxesComponent.getY(),
                                          getWidth() * 2,
                                          getHeight() * 2);
    }
}

void TrackPlayer::flexBoxInit()
{
    trackPlayerFlexBox.flexDirection = juce::FlexBox::Direction::column;
    trackPlayerFlexBox.flexWrap = juce::FlexBox::Wrap::noWrap;

    clipsBoxesFlexBox.flexDirection = juce::FlexBox::Direction::column;
    clipsBoxesFlexBox.flexWrap = juce::FlexBox::Wrap::noWrap;
    std::cout << getParentHeight() << std::endl;

    trackPlayerFlexBox.items.add(juce::FlexItem(timeline)
                                     .withFlex(0, 1, TrackPlayerConstants::timelineHeightRatio * getParentHeight())
                                     .withMinHeight(TrackPlayerConstants::minTimelineHeightRatio * getParentHeight()));
    trackPlayerFlexBox.items.add(juce::FlexItem(clipsBoxesFlexBox).withFlex(1));

    clipsBoxesFlexBox.items.add(juce::FlexItem(clipsBoxesComponent));
}

void TrackPlayer::drawBoxes()
{
    clipsBoxesVector.clear();
    for(auto i{0}; i < 20; i++)
    {
        float x{0.0f};
        float y{static_cast<float>(clipsBoxesComponent.getY())};
        auto clipsBox = std::make_unique<ClipsBox>(x, y, tempNumOfBoxes);
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
