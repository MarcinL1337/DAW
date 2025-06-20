#include "TrackPlayerSideMenu.h"
#include "../TrackManager.h"

TrackPlayerSideMenu::TrackPlayerSideMenu(juce::ValueTree& parentTree) : tree(parentTree) { tree.addListener(this); }

void TrackPlayerSideMenu::paint(juce::Graphics& g)
{
    g.setColour(juce::Colours::darkgrey);
    g.fillRect(0, currentSelectedTrack * currentTrackGuiBoxHeight, getWidth(), currentTrackGuiBoxHeight);

    g.setColour(juce::Colours::whitesmoke);
    for(auto i{0u}; i < getCurrentNumberOfTracks() + 1; ++i)
    {
        g.drawLine(0, i * currentTrackGuiBoxHeight, getWidth(), i * currentTrackGuiBoxHeight, 0.75);
    }
}

void TrackPlayerSideMenu::resized()
{
    trackButtonsSize = static_cast<uint16_t>(0.15 * getWidth());
    buttonMargin = static_cast<uint16_t>(0.01 * getWidth());
}

void TrackPlayerSideMenu::valueTreePropertyChanged(juce::ValueTree&, const juce::Identifier& property)
{
    if(static_cast<int>(tree[property]) == ValueTreeConstants::doNothing)
        return;

    if(property == ValueTreeIDs::setSelectedTrack)
    {
        currentSelectedTrack = tree[ValueTreeIDs::setSelectedTrack];
        repaint();
    }
}

void TrackPlayerSideMenu::mouseDown(const juce::MouseEvent& event)
{
    if(event.mods.isLeftButtonDown())
    {
        const int trackIndexClicked = event.getMouseDownY() / currentTrackGuiBoxHeight;
        if(trackIndexClicked < getCurrentNumberOfTracks())
        {
            tree.setProperty(ValueTreeIDs::setSelectedTrack, trackIndexClicked, nullptr);
        }
    }
}

void TrackPlayerSideMenu::resizeAllTrackButtons(const int newBoxHeight)
{
    currentTrackGuiBoxHeight = newBoxHeight;

    int currentRow{0};
    for(auto& [recordButton, soloButton, muteButton, _]: trackControlsVector)
    {
        juce::Rectangle<int> currentTrackButtonsArea{
            getWidth() / 2, currentRow * currentTrackGuiBoxHeight, getWidth() / 2, currentTrackGuiBoxHeight};

        recordButton->setBounds(currentTrackButtonsArea.removeFromRight(trackButtonsSize)
                                    .withSizeKeepingCentre(trackButtonsSize, trackButtonsSize)
                                    .reduced(buttonMargin));

        soloButton->setBounds(currentTrackButtonsArea.removeFromRight(trackButtonsSize)
                                  .withSizeKeepingCentre(trackButtonsSize, trackButtonsSize)
                                  .reduced(buttonMargin));

        muteButton->setBounds(currentTrackButtonsArea.removeFromRight(trackButtonsSize)
                                  .withSizeKeepingCentre(trackButtonsSize, trackButtonsSize)
                                  .reduced(buttonMargin));

        currentRow++;
    }
}

void TrackPlayerSideMenu::addTrackControls()
{
    const int currentRow{getCurrentNumberOfTracks()};
    auto currentTrackButtonsArea = getCurrentTrackButtonsArea(currentRow);
    auto currentTrackNameArea = getCurrentTrackNameArea(currentRow);

    auto recordButton = std::make_unique<juce::TextButton>("R");
    auto soloButton = std::make_unique<juce::TextButton>("S");
    auto muteButton = std::make_unique<juce::TextButton>("M");
    auto trackNameLabel = std::make_unique<juce::Label>("", "Track nr " + std::to_string(currentRow + 1));

    setupRecordButton(recordButton, currentTrackButtonsArea, currentRow);
    setupSoloButton(soloButton, currentTrackButtonsArea, currentRow);
    setupMuteButton(muteButton, currentTrackButtonsArea, currentRow);
    setupTrackNameLabel(trackNameLabel, currentTrackNameArea);

    trackControlsVector.push_back(
        {std::move(recordButton), std::move(soloButton), std::move(muteButton), std::move(trackNameLabel)});

    addAndMakeVisible(trackControlsVector.back().recordButton.get());
    addAndMakeVisible(trackControlsVector.back().soloButton.get());
    addAndMakeVisible(trackControlsVector.back().muteButton.get());
    addAndMakeVisible(trackControlsVector.back().trackNameLabel.get());

    incrementCurrentNumberOfTracks();
}

void TrackPlayerSideMenu::setupRecordButton(const std::unique_ptr<juce::TextButton>& recordButton,
                                            juce::Rectangle<int>& buttonArea, const uint16_t currentRow)
{
    recordButton->setBounds(buttonArea.removeFromRight(trackButtonsSize)
                                .withSizeKeepingCentre(trackButtonsSize, trackButtonsSize)
                                .reduced(buttonMargin));
    recordButton->onClick = [this, currentRow]()
    {
        std::cout << "Recording[" << currentRow + 1 << "]" << std::endl;
        tree.setProperty(ValueTreeIDs::setSelectedTrack, currentRow, nullptr);
    };
}

void TrackPlayerSideMenu::setupSoloButton(const std::unique_ptr<juce::TextButton>& soloButton,
                                          juce::Rectangle<int>& buttonArea, const uint16_t currentRow)
{
    soloButton->setBounds(buttonArea.removeFromRight(trackButtonsSize)
                              .withSizeKeepingCentre(trackButtonsSize, trackButtonsSize)
                              .reduced(buttonMargin));
    soloButton->onClick = [this, currentRow, soloButtonPtr = soloButton.get()]()
    {
        tree.setProperty(ValueTreeIDs::soloButtonClicked, ValueTreeConstants::doNothing, nullptr);
        tree.setProperty(ValueTreeIDs::soloButtonClicked, currentRow, nullptr);
        tree.setProperty(ValueTreeIDs::setSelectedTrack, currentRow, nullptr);
        const bool currentState = soloButtonPtr->getToggleState();
        soloButtonPtr->setToggleState(!currentState, juce::dontSendNotification);
    };
}

void TrackPlayerSideMenu::setupMuteButton(const std::unique_ptr<juce::TextButton>& muteButton,
                                          juce::Rectangle<int>& buttonArea, const uint16_t currentRow)
{
    muteButton->setBounds(buttonArea.removeFromRight(trackButtonsSize)
                              .withSizeKeepingCentre(trackButtonsSize, trackButtonsSize)
                              .reduced(buttonMargin));
    muteButton->onClick = [this, currentRow, muteButtonPtr = muteButton.get()]()
    {
        tree.setProperty(ValueTreeIDs::muteButtonClicked, ValueTreeConstants::doNothing, nullptr);
        tree.setProperty(ValueTreeIDs::muteButtonClicked, currentRow, nullptr);
        tree.setProperty(ValueTreeIDs::setSelectedTrack, currentRow, nullptr);
        const bool currentState = muteButtonPtr->getToggleState();
        muteButtonPtr->setToggleState(!currentState, juce::dontSendNotification);
    };
}

void TrackPlayerSideMenu::setupTrackNameLabel(const std::unique_ptr<juce::Label>& trackNameLabel,
                                              juce::Rectangle<int>& trackNameArea)
{
    trackNameLabel->setBounds(trackNameArea.removeFromRight(0.9 * trackNameArea.getWidth()));
    trackNameLabel->setEditable(false, true, false);
}

juce::Rectangle<int> TrackPlayerSideMenu::getCurrentTrackButtonsArea(const uint16_t currentRow) const
{
    return juce::Rectangle<int>{
        getWidth() / 2, currentRow * currentTrackGuiBoxHeight, getWidth() / 2, currentTrackGuiBoxHeight};
}

juce::Rectangle<int> TrackPlayerSideMenu::getCurrentTrackNameArea(const uint16_t currentRow) const
{
    return juce::Rectangle<int>{0,
                                currentRow * currentTrackGuiBoxHeight + currentTrackGuiBoxHeight / 4,
                                getWidth() / 2,
                                currentTrackGuiBoxHeight / 2};
}

void TrackPlayerSideMenu::removeTrackControls(int trackIndex)
{
    trackControlsVector.erase(trackControlsVector.begin() + trackIndex);
    resizeAllTrackButtons(currentTrackGuiBoxHeight);
    for(; trackIndex < trackControlsVector.size(); trackIndex++)
    {
        auto& [recordButton, soloButton, muteButton, trackNameLabel] = trackControlsVector[trackIndex];
        auto currentTrackButtonsArea = getCurrentTrackButtonsArea(trackIndex);
        auto currentTrackNameArea = getCurrentTrackNameArea(trackIndex);
        setupRecordButton(recordButton, currentTrackButtonsArea, trackIndex);
        setupSoloButton(soloButton, currentTrackButtonsArea, trackIndex);
        setupMuteButton(muteButton, currentTrackButtonsArea, trackIndex);
        setupTrackNameLabel(trackNameLabel, currentTrackNameArea);
    }
    decrementCurrentNumberOfTracks();
}

void TrackPlayerSideMenu::updateTrackButtonStates(const int trackIndex, const bool muted, const bool soloed) const
{
    assert(trackIndex >= 0 && trackIndex < trackControlsVector.size());
    trackControlsVector[trackIndex].muteButton->setToggleState(muted, juce::dontSendNotification);
    trackControlsVector[trackIndex].soloButton->setToggleState(soloed, juce::dontSendNotification);
}
