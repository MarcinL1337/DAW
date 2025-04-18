#include "MainComponent.h"

MainComponent::MainComponent() :
    mainToolbar(mainAudio, tree), trackPlayer(tree, trackManager), trackManager(trackPlayer, mainAudio)
{
    // 2560 x 1392 = Total screen width x (Total screen height - (windows bar size + title bar size))
    setSize(getParentWidth(), getParentHeight());
    addAndMakeVisible(topLevelMenu);
    addAndMakeVisible(mainToolbar);
    addAndMakeVisible(trackPlayer);

    addAndMakeVisible(sideMenu);
    flexBoxInit();

    addKeyListener(this);
    juce::Timer::callAfterDelay(50, [&] { addTestTracks(); });
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

    trackPlayerFlexBox.items.add(juce::FlexItem(trackPlayer).withFlex(1));
}

// This function is for testing. It can be long. Will be deleted soon
void MainComponent::addTestTracks()
{
    const juce::File dawDir =
        juce::File::getCurrentWorkingDirectory().getParentDirectory().getParentDirectory().getParentDirectory();
    const juce::File countdownAudioFile = dawDir.getChildFile("Assets/Audio/countdown.wav");
    const juce::File musicAudioFile = dawDir.getChildFile("Assets/Audio/test.wav");
    const juce::File invertedMusicAudioFile = dawDir.getChildFile("Assets/Audio/test(-1).wav");
    const juce::File mutedMusicAudioFile = dawDir.getChildFile("Assets/Audio/test.mp3");
    auto trackId = trackManager.addTrack();

    if(countdownAudioFile.existsAsFile())
    {
        const auto audioClipId = trackManager.addAudioClipToTrack(trackId, countdownAudioFile);
        trackManager.setOffsetOfAudioClipInSeconds(audioClipId, 0.0f);
        trackManager.setPropertyForAllClipsInTrack(trackId, AudioClipProperty::PAN, 0.5f);
        trackManager.setPropertyForAllClipsInTrack(trackId, AudioClipProperty::GAIN, -5.0f);
    }
    else
        juce::AlertWindow::showMessageBoxAsync(
            juce::AlertWindow::WarningIcon, "Error", countdownAudioFile.getFullPathName() + " not found :c");

    if(musicAudioFile.existsAsFile())
    {
        trackId = trackManager.addTrack();
        const auto audioClipId = trackManager.addAudioClipToTrack(trackId, musicAudioFile);
        trackManager.setOffsetOfAudioClipInSeconds(audioClipId, 6.0f);
        trackManager.setPropertyForAllClipsInTrack(trackId, AudioClipProperty::PAN, 0.0f);
        trackManager.setPropertyForAllClipsInTrack(trackId, AudioClipProperty::GAIN, -15.0f);
    }
    else
        juce::AlertWindow::showMessageBoxAsync(
            juce::AlertWindow::WarningIcon, "Error", musicAudioFile.getFullPathName() + " not found :c");

    if(invertedMusicAudioFile.existsAsFile())
    {
        trackId = trackManager.addTrack();
        const auto audioClipId = trackManager.addAudioClipToTrack(trackId, invertedMusicAudioFile);
        trackManager.setOffsetOfAudioClipInSeconds(audioClipId, 6.0f);
        trackManager.setPropertyForAllClipsInTrack(trackId, AudioClipProperty::PAN, 0.6f);
        trackManager.setPropertyForAllClipsInTrack(trackId, AudioClipProperty::GAIN, -15.0f);
    }
    else
        juce::AlertWindow::showMessageBoxAsync(
            juce::AlertWindow::WarningIcon, "Error", invertedMusicAudioFile.getFullPathName() + " not found :c");

    if(mutedMusicAudioFile.existsAsFile())
    {
        trackId = trackManager.addTrack();
        const auto audioClipId = trackManager.addAudioClipToTrack(trackId, mutedMusicAudioFile);
        trackManager.setOffsetOfAudioClipInSeconds(audioClipId, 1.0f);
        trackManager.setPropertyForAllClipsInTrack(trackId, AudioClipProperty::PAN, 0.0f);
        trackManager.setPropertyForAllClipsInTrack(trackId, AudioClipProperty::GAIN, -15.0f);
        trackManager.setPropertyForAllClipsInTrack(trackId, AudioClipProperty::MUTE, false);
        trackManager.setPropertyForAllClipsInTrack(trackId, AudioClipProperty::SOLO, false);
    }
    else
        juce::AlertWindow::showMessageBoxAsync(
            juce::AlertWindow::WarningIcon, "Error", mutedMusicAudioFile.getFullPathName() + " not found :c");
}

bool MainComponent::keyPressed(const juce::KeyPress& key, Component* originatingComponent)
{
    return trackManager.keyPressed(key);
}

