#include "TrackPlayerSideMenu.h"
#include "../TrackManager.h"

TrackPlayerSideMenu::TrackPlayerSideMenu(const juce::ValueTree& parentTree) : tree(parentTree) {}

void TrackPlayerSideMenu::paint(juce::Graphics& g)
{
    g.setColour(juce::Colours::whitesmoke);

    for(auto i{0u}; i < getCurrentNumberOfTracks() + 1; ++i)
    {
        g.drawLine(0, i * currentTrackGuiBoxHeight, getWidth(), i * currentTrackGuiBoxHeight, 0.75);
    }
    drawTrackText(g);
}

void TrackPlayerSideMenu::resized()
{
    trackButtonsSize = static_cast<uint16_t>(0.15 * getWidth());
    buttonMargin = static_cast<uint16_t>(0.01 * getWidth());
}

void TrackPlayerSideMenu::drawTrackText(juce::Graphics& g) const
{
    for(auto i{0}; i < getCurrentNumberOfTracks(); i++)
    {
        juce::Rectangle<int> currentTrackTextArea{
            0, i * currentTrackGuiBoxHeight, getWidth() / 2, currentTrackGuiBoxHeight};
        g.drawFittedText("Track nr " + std::to_string(i + 1),
                         currentTrackTextArea.removeFromRight(0.9 * currentTrackTextArea.getWidth()),
                         juce::Justification::centredLeft,
                         2);
    }
}

void TrackPlayerSideMenu::changeCurrentTrackGuiBoxHeight(const int newBoxHeight)
{
    currentTrackGuiBoxHeight = newBoxHeight;

    int currentRow{0};
    for(auto& [recordButton, soloButton, muteButton]: trackButtonsVector)
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
    auto recordButton = std::make_unique<juce::TextButton>("R");
    auto soloButton = std::make_unique<juce::TextButton>("S");
    auto muteButton = std::make_unique<juce::TextButton>("M");

    const int currentRow{getCurrentNumberOfTracks()};
    juce::Rectangle<int> currentTrackButtonsArea{
        getWidth() / 2, currentRow * currentTrackGuiBoxHeight, getWidth() / 2, currentTrackGuiBoxHeight};

    initRecordButton(recordButton, currentTrackButtonsArea, currentRow);
    initSoloButton(soloButton, currentTrackButtonsArea, currentRow);
    initMuteButton(muteButton, currentTrackButtonsArea, currentRow);

    trackButtonsVector.push_back({std::move(recordButton), std::move(soloButton), std::move(muteButton)});
    for(auto& button: trackButtonsVector.back()) { addAndMakeVisible(button.get()); }

    incrementCurrentNumberOfTracks();
}

void TrackPlayerSideMenu::initRecordButton(const std::unique_ptr<juce::TextButton>& recordButton,
                                           juce::Rectangle<int>& buttonArea, const uint16_t currentRow)
{
    recordButton->setBounds(buttonArea.removeFromRight(trackButtonsSize)
                                .withSizeKeepingCentre(trackButtonsSize, trackButtonsSize)
                                .reduced(buttonMargin));
    recordButton->onClick = [this, currentRow]() { std::cout << "Recording[" << currentRow + 1 << "]" << std::endl; };
}

void TrackPlayerSideMenu::initSoloButton(const std::unique_ptr<juce::TextButton>& soloButton,
                                         juce::Rectangle<int>& buttonArea, const uint16_t currentRow)
{
    soloButton->setBounds(buttonArea.removeFromRight(trackButtonsSize)
                              .withSizeKeepingCentre(trackButtonsSize, trackButtonsSize)
                              .reduced(buttonMargin));
    soloButton->onClick = [this, currentRow, soloButtonPtr = soloButton.get()]()
    {
        tree.setProperty("soloButtonClicked", ValueTreeConstants::doNothing, nullptr);
        tree.setProperty("soloButtonClicked", currentRow, nullptr);
        const bool currentState = soloButtonPtr->getToggleState();
        soloButtonPtr->setToggleState(!currentState, juce::dontSendNotification);
    };
}

void TrackPlayerSideMenu::initMuteButton(const std::unique_ptr<juce::TextButton>& muteButton,
                                         juce::Rectangle<int>& buttonArea, const uint16_t currentRow)
{
    muteButton->setBounds(buttonArea.removeFromRight(trackButtonsSize)
                              .withSizeKeepingCentre(trackButtonsSize, trackButtonsSize)
                              .reduced(buttonMargin));
    muteButton->onClick = [this, currentRow, muteButtonPtr = muteButton.get()]()
    {
        tree.setProperty("muteButtonClicked", ValueTreeConstants::doNothing, nullptr);
        tree.setProperty("muteButtonClicked", currentRow, nullptr);
        const bool currentState = muteButtonPtr->getToggleState();
        muteButtonPtr->setToggleState(!currentState, juce::dontSendNotification);
    };
}

void TrackPlayerSideMenu::removeTrackControls(const int trackIndex)
{
    trackButtonsVector.erase(trackButtonsVector.begin() + trackIndex);
    decrementCurrentNumberOfTracks();
}

