#include "MainComponent.h"

MainComponent::MainComponent()
{
    addAndMakeVisible(topLevelMenu);
    addAndMakeVisible(mainToolbar);
    addAndMakeVisible(trackPlayer);
    addAndMakeVisible(timeline);
    addAndMakeVisible(sideMenu);
    flexBoxInit();
}

void MainComponent::paint(juce::Graphics& g)
{
    g.fillAll(getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId));
}

void MainComponent::resized() { topLevelFlexBox.performLayout(getLocalBounds()); }

void MainComponent::flexBoxInit()
{
    constexpr int timelineHeight{50};
    const float trackPlayerHeight =
        getParentHeight() - (topLevelMenuHeight + mainToolbarHeight + timelineHeight);  // temporary

    topLevelFlexBox.flexDirection = juce::FlexBox::Direction::column;
    topLevelFlexBox.flexWrap = juce::FlexBox::Wrap::noWrap;

    mainContentFlexBox.flexDirection = juce::FlexBox::Direction::row;

    trackPlayerFlexBox.flexDirection = juce::FlexBox::Direction::column;
    trackPlayerFlexBox.flexWrap = juce::FlexBox::Wrap::noWrap;

    topLevelFlexBox.items.add(juce::FlexItem(0, topLevelMenuHeight, topLevelMenu));
    topLevelFlexBox.items.add(juce::FlexItem(0, mainToolbarHeight, mainToolbar));
    topLevelFlexBox.items.add(juce::FlexItem(0, trackPlayerHeight + timelineHeight, mainContentFlexBox).withFlex(1.0));

    mainContentFlexBox.items.add(juce::FlexItem(sideMenuWidth, 1299, sideMenu));
    mainContentFlexBox.items.add(juce::FlexItem(2560.0f - sideMenuWidth, 0, trackPlayerFlexBox));

    trackPlayerFlexBox.items.add(juce::FlexItem(0, timelineHeight, timeline));
    trackPlayerFlexBox.items.add(juce::FlexItem(0, /* 1369 - pozostałe */ 1249, trackPlayer));
}
