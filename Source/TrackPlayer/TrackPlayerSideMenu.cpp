#include "TrackPlayerSideMenu.h"
#include "../TrackManager.h"

TrackPlayerSideMenu::TrackPlayerSideMenu(const juce::ValueTree& parentTree) : tree(parentTree) {}

// TODO: Styling for track text and buttons is still not great -> fix with free time

void TrackPlayerSideMenu::paint(juce::Graphics& g)
{
    g.setColour(juce::Colours::whitesmoke);

    for(auto i{0u}; i < getCurrentNumberOfTracks() + 1; ++i)
    {
        g.drawLine(0, i * currentTrackGuiBoxHeight, getWidth(), i * currentTrackGuiBoxHeight, 0.75);
    }
    drawTrackText(g);
}

void TrackPlayerSideMenu::resized() { drawTrackButtons(); }

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

void TrackPlayerSideMenu::drawTrackButtons()
{
    trackButtonsVector.clear();
    for(auto i{0}; i < getCurrentNumberOfTracks(); i++)
    {
        juce::Rectangle<int> currentTrackButtonsArea{
            getWidth() / 2, i * currentTrackGuiBoxHeight, getWidth() / 2, currentTrackGuiBoxHeight};

        auto recordButton = std::make_unique<juce::TextButton>("R");
        auto soloButton = std::make_unique<juce::TextButton>("S");
        auto muteButton = std::make_unique<juce::TextButton>("M");

        recordButton->setBounds(currentTrackButtonsArea.removeFromRight(trackButtonsSize)
                                    .withSizeKeepingCentre(trackButtonsSize, trackButtonsSize)
                                    .reduced(buttonMargin));
        recordButton->onClick = [i]() { std::cout << "Recording[" << i + 1 << "]" << std::endl; };

        soloButton->setBounds(currentTrackButtonsArea.removeFromRight(trackButtonsSize)
                                          .withSizeKeepingCentre(trackButtonsSize, trackButtonsSize)
                                          .reduced(buttonMargin));
        soloButton->onClick = [this, i, soloButtonPtr = soloButton.get()]()
        {
            const bool currentState = soloButtonPtr->getToggleState();
            soloButtonPtr->setToggleState(!currentState, juce::dontSendNotification);
            tree.setProperty("trackAction", "SOLO", nullptr);
            tree.setProperty("trackActionIndex", i, nullptr);
            tree.setProperty("trackActionValue", !currentState, nullptr);
            std::cout << "Soloing[" << i + 1 << "] - " << (!currentState ? "ON" : "OFF") << std::endl;
        };
        muteButton->setBounds(currentTrackButtonsArea.removeFromRight(trackButtonsSize)
                                          .withSizeKeepingCentre(trackButtonsSize, trackButtonsSize)
                                          .reduced(buttonMargin));
        muteButton->onClick = [this, i, muteButtonPtr = muteButton.get()]()
        {
            const bool currentState = muteButtonPtr->getToggleState();
            muteButtonPtr->setToggleState(!currentState, juce::dontSendNotification);
            tree.setProperty("trackAction", "MUTE", nullptr);
            tree.setProperty("trackActionIndex", i, nullptr);
            // TODO: trackActionValue should be deleted
            tree.setProperty("trackActionValue", !currentState, nullptr);
            std::cout << "Muting[" << i + 1 << "] - " << (!currentState ? "ON" : "OFF") << std::endl;
        };

        trackButtonsVector.push_back({std::move(recordButton), std::move(soloButton), std::move(muteButton)});
        for(auto& button: trackButtonsVector.back()) { addAndMakeVisible(button.get()); }
    }
}
