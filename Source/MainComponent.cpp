#include "MainComponent.h"

MainComponent::MainComponent() :
    mainAudio{tree},
    topLevelMenu{tree},
    mainToolbar{tree},
    trackPlayer{tree},
    sideMenu{tree},
    trackManager{trackPlayer, mainAudio, sideMenu},
    projectFilesManager{tree}
{
    // 2560 x 1392 = Total screen width x (Total screen height - (windows bar size + title bar size))
    setSize(getParentWidth(), getParentHeight());
    addAndMakeVisible(topLevelMenu);
    addAndMakeVisible(mainToolbar);
    addAndMakeVisible(trackPlayer);
    addAndMakeVisible(sideMenu);
    addAndMakeVisible(trackManager);
    flexBoxInit();

    // projectFilesManager.openTestProject();
    setWantsKeyboardFocus(true);
}

void MainComponent::paint(juce::Graphics& g)
{
    g.fillAll(getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId));
}

void MainComponent::resized() { topLevelFlexBox.performLayout(getLocalBounds()); }

bool MainComponent::keyPressed(const juce::KeyPress& key)
{
    if(key == juce::KeyPress('n', juce::ModifierKeys::ctrlModifier, 0))
    {
        return topLevelMenu.keyPressed(key);
    }

    if(key == juce::KeyPress('o', juce::ModifierKeys::ctrlModifier, 0))
    {
        return topLevelMenu.keyPressed(key);
    }

    if(key == juce::KeyPress('s', juce::ModifierKeys::ctrlModifier, 0))
    {
        return topLevelMenu.keyPressed(key);
    }

    if(key == juce::KeyPress('s', juce::ModifierKeys::shiftModifier | juce::ModifierKeys::ctrlModifier, 0))
    {
        return topLevelMenu.keyPressed(key);
    }

    if(key == juce::KeyPress('a', juce::ModifierKeys::shiftModifier | juce::ModifierKeys::ctrlModifier, 0))
    {
        return topLevelMenu.keyPressed(key);
    }

    if(key == juce::KeyPress('e', juce::ModifierKeys::shiftModifier | juce::ModifierKeys::ctrlModifier, 0))
    {
        return topLevelMenu.keyPressed(key);
    }

    if(key == juce::KeyPress('h', juce::ModifierKeys::ctrlModifier, 0))
    {
        return topLevelMenu.keyPressed(key);
    }

    if(key == juce::KeyPress('=', juce::ModifierKeys::shiftModifier, 0))
    {
        trackManager.addTrack();
        return true;
    }

    if(key == juce::KeyPress::spaceKey)
    {
        mainToolbar.toolbarFactory.playPauseButtonClicked();
        return true;
    }

    if(key == juce::KeyPress::backspaceKey)
    {
        mainToolbar.toolbarFactory.stopButtonClicked();
        return true;
    }

    return false;
}

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

    trackPlayerFlexBox.items.add(juce::FlexItem(trackPlayer).withFlex(1));
}
