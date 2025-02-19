#include "MainComponent.h"

MainComponent::MainComponent() : mainToolbar(mainAudio)
{
    // 2560 x 1392 = Total screen width x (Total screen height - (windows bar size + title bar size))
    setSize(getParentWidth(), getParentHeight());
    addAndMakeVisible(topLevelMenu);
    addAndMakeVisible(mainToolbar);
    addAndMakeVisible(trackPlayer);
    addAndMakeVisible(timeline);
    addAndMakeVisible(sideMenu);
    flexBoxInit();
    addTestTrack();
}

void MainComponent::paint(juce::Graphics& g)
{
    g.fillAll(getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId));
}

void MainComponent::resized() { topLevelFlexBox.performLayout(getLocalBounds()); }

void MainComponent::flexBoxInit()
{
    topLevelFlexBox.flexDirection = juce::FlexBox::Direction::column;
    topLevelFlexBox.flexWrap = juce::FlexBox::Wrap::noWrap;

    mainContentFlexBox.flexDirection = juce::FlexBox::Direction::row;

    trackPlayerFlexBox.flexDirection = juce::FlexBox::Direction::column;
    trackPlayerFlexBox.flexWrap = juce::FlexBox::Wrap::noWrap;

    topLevelFlexBox.items.add(juce::FlexItem(topLevelMenu)
                                  .withFlex(0, 1, TopLevelMenuConstants::topLevelMenuHeightRatio * getParentHeight()));
    topLevelFlexBox.items.add(
        juce::FlexItem(mainToolbar).withFlex(0, 1, MainToolbarConstants::mainToolbarHeightRatio * getParentHeight()));
    topLevelFlexBox.items.add(juce::FlexItem(mainContentFlexBox).withFlex(1));

    mainContentFlexBox.items.add(
        juce::FlexItem(sideMenu).withFlex(0, 1, SideMenuConstants::sideMenuWidthRatio * getWidth()));
    mainContentFlexBox.items.add(juce::FlexItem(trackPlayerFlexBox).withFlex(1));

    trackPlayerFlexBox.items.add(juce::FlexItem(timeline)
                                     .withFlex(0, 1, TrackPlayerConstants::timelineHeightRatio * getParentHeight())
                                     .withMinHeight(TrackPlayerConstants::minTimelineHeightRatio * getParentHeight()));
    trackPlayerFlexBox.items.add(
        juce::FlexItem(trackPlayer).withFlex(0, 1, TrackPlayerConstants::trackPlayerHeightRatio * getParentHeight()));
}

void MainComponent::addTestTrack()
{
    const juce::File audioFile("C:/Users/julek/CLionProjects/DAW/Source/Audio/test.mp3");

    if(audioFile.existsAsFile())
        mainAudio.addTrack(audioFile);
    else
        juce::AlertWindow::showMessageBoxAsync(juce::AlertWindow::WarningIcon, "Error", "test.wav not found");
}