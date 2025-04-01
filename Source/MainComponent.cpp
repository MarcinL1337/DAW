#include "MainComponent.h"

MainComponent::MainComponent() : mainToolbar(mainAudio), trackManager(mainAudio, trackPlayer)
{
    // 2560 x 1392 = Total screen width x (Total screen height - (windows bar size + title bar size))
    setSize(getParentWidth(), getParentHeight());
    addAndMakeVisible(topLevelMenu);
    addAndMakeVisible(mainToolbar);
    addAndMakeVisible(trackPlayer);

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

    // Dodanie pierwszego tracka wizualnego (jeden już istnieje domyślnie)
    int firstTrackIndex = 0;

    // Dodanie drugiego tracka wizualnego
    int secondTrackIndex = trackManager.addVisualTrack();

    // Dodanie trzeciego tracka wizualnego
    int thirdTrackIndex = trackManager.addVisualTrack();

    if(countdownAudioFile.existsAsFile())
    {
        // Dodaj track do pierwszego tracka wizualnego
        auto trackId = trackManager.addAudioTrack(countdownAudioFile);
        trackManager.assignAudioToTrack(trackId, firstTrackIndex);
        trackManager.setPanOfTrack(trackId, 0.5f);
        trackManager.setGainOfTrack(trackId, -5.0f);
        trackManager.setOffsetOfTrackInSeconds(trackId, 0.0f);
    }
    else
        juce::AlertWindow::showMessageBoxAsync(
            juce::AlertWindow::WarningIcon, "Error", countdownAudioFile.getFullPathName() + " not found :c");

    if(musicAudioFile.existsAsFile())
    {
        // Dodaj track do drugiego tracka wizualnego
        auto trackId = trackManager.addAudioTrack(musicAudioFile);
        trackManager.assignAudioToTrack(trackId, secondTrackIndex);
        trackManager.setPanOfTrack(trackId, 0);
        trackManager.setGainOfTrack(trackId, -15.0f);
        trackManager.setOffsetOfTrackInSeconds(trackId, 6.0f);
    }
    else
        juce::AlertWindow::showMessageBoxAsync(
            juce::AlertWindow::WarningIcon, "Error", musicAudioFile.getFullPathName() + " not found :c");

    if(invertedMusicAudioFile.existsAsFile())
    {
        // Dodaj track do drugiego tracka wizualnego (jako drugi audioclip w tym tracku)
        auto trackId = trackManager.addAudioTrack(invertedMusicAudioFile);
        trackManager.assignAudioToTrack(trackId, secondTrackIndex);
        trackManager.setPanOfTrack(trackId, 0.6);
        trackManager.setGainOfTrack(trackId, -15.0f);
        trackManager.setOffsetOfTrackInSeconds(trackId, 6.0f);
    }
    else
        juce::AlertWindow::showMessageBoxAsync(
            juce::AlertWindow::WarningIcon, "Error", invertedMusicAudioFile.getFullPathName() + " not found :c");

    if(mutedMusicAudioFile.existsAsFile())
    {
        // Dodaj track do trzeciego tracka wizualnego
        auto trackId = trackManager.addAudioTrack(mutedMusicAudioFile);
        trackManager.assignAudioToTrack(trackId, thirdTrackIndex);
        trackManager.setPanOfTrack(trackId, 0);
        trackManager.setGainOfTrack(trackId, -15.0f);
        trackManager.setOffsetOfTrackInSeconds(trackId, 1.0f);
        trackManager.setMuteOfTrack(trackId, true);
        trackManager.setSoloOfTrack(trackId, false);
    }
    else
        juce::AlertWindow::showMessageBoxAsync(
            juce::AlertWindow::WarningIcon, "Error", mutedMusicAudioFile.getFullPathName() + " not found :c");
}