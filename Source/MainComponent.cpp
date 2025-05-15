#include "MainComponent.h"

MainComponent::MainComponent() :
    mainAudio(tree),
    topLevelMenu(tree),
    mainToolbar(tree),
    trackPlayer(tree),
    trackManager(trackPlayer, mainAudio, sideMenu),
    sideMenu(tree)
{
    // 2560 x 1392 = Total screen width x (Total screen height - (windows bar size + title bar size))
    setSize(getParentWidth(), getParentHeight());
    addAndMakeVisible(topLevelMenu);
    addAndMakeVisible(mainToolbar);
    addAndMakeVisible(trackPlayer);
    addAndMakeVisible(trackManager);

    addAndMakeVisible(sideMenu);
    flexBoxInit();

    addTestTracks();
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

    auto errorMsg = [](const juce::String& msg)
    { juce::AlertWindow::showMessageBoxAsync(juce::AlertWindow::WarningIcon, "Error", msg + " not found :c"); };

    auto trackIndex = trackManager.addTrack();

    if(countdownAudioFile.existsAsFile())
    {
        const auto audioClipId = trackManager.addAudioClipToTrack(trackIndex, countdownAudioFile);
        trackManager.setOffsetOfAudioClipInSeconds(audioClipId, 0.0f);
        trackManager.setTrackProperty(trackIndex, AudioClipProperty::PAN, 0.0f);
        trackManager.setTrackProperty(trackIndex, AudioClipProperty::GAIN, -15.0f);
    }
    else
        errorMsg(countdownAudioFile.getFullPathName());

    if(musicAudioFile.existsAsFile())
    {
        const auto audioClipId = trackManager.addAudioClipToTrack(trackIndex, musicAudioFile);
        trackManager.setOffsetOfAudioClipInSeconds(audioClipId, 6.0f);
    }
    else
        errorMsg(musicAudioFile.getFullPathName());

    if(invertedMusicAudioFile.existsAsFile())
    {
        trackIndex = trackManager.addTrack();
        const auto audioClipId = trackManager.addAudioClipToTrack(trackIndex, invertedMusicAudioFile);
        trackManager.setOffsetOfAudioClipInSeconds(audioClipId, 6.0f);
        trackManager.setTrackProperty(trackIndex, AudioClipProperty::PAN, 0.6f);
        trackManager.setTrackProperty(trackIndex, AudioClipProperty::GAIN, -15.0f);
    }
    else
        errorMsg(invertedMusicAudioFile.getFullPathName());

    if(mutedMusicAudioFile.existsAsFile())
    {
        trackIndex = trackManager.addTrack();
        const auto audioClipId = trackManager.addAudioClipToTrack(trackIndex, mutedMusicAudioFile);
        trackManager.setOffsetOfAudioClipInSeconds(audioClipId, 1.0f);
        trackManager.setTrackProperty(trackIndex, AudioClipProperty::PAN, 0.0f);
        trackManager.setTrackProperty(trackIndex, AudioClipProperty::GAIN, -15.0f);
        trackManager.setTrackProperty(trackIndex, AudioClipProperty::MUTE, false);
        trackManager.setTrackProperty(trackIndex, AudioClipProperty::SOLO, false);
    }
    else
        errorMsg(mutedMusicAudioFile.getFullPathName());
}
