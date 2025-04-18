#include "TrackPlayerSideMenu.h"
#include "../TrackManager.h"

TrackPlayerSideMenu::TrackPlayerSideMenu(TrackManager& trackManagerRef) : trackManager(trackManagerRef) {}

void TrackPlayerSideMenu::paint(juce::Graphics& g)
{
    g.setColour(juce::Colours::whitesmoke);

    for(auto i{0u}; i < getCurrentNumberOfTracks() + 1; ++i)
    {
        g.drawLine(
            0, i * TrackPlayerConstants::startBoxHeight, getWidth(), i * TrackPlayerConstants::startBoxHeight, 0.75);
    }
    drawTrackText(g);
}

void TrackPlayerSideMenu::resized() { drawTrackButtons(); }

void TrackPlayerSideMenu::drawTrackText(juce::Graphics& g) const
{
    for(auto i{0u}; i < getCurrentNumberOfTracks(); i++)
    {
        auto currentY{i * TrackPlayerConstants::startBoxHeight + 15};
        g.setColour(juce::Colours::white);
        g.drawText("Track nr " + std::to_string(i + 1),
                   10,
                   currentY,
                   100,
                   trackButtonsSize,
                   juce::Justification::centred,
                   false);
    }
}

void TrackPlayerSideMenu::drawTrackButtons()
{
    trackButtonsVector.clear();
    const auto startX{getWidth() - 2 * trackButtonsSize};
    const auto xDifference{trackButtonsSize + 5};
    for(auto i{0u}; i < getCurrentNumberOfTracks(); i++)
    {
        auto recordButton = std::make_unique<juce::TextButton>("R");
        auto soloButton = std::make_unique<juce::TextButton>("S");
        auto muteButton = std::make_unique<juce::TextButton>("M");

        auto currentY{i * TrackPlayerConstants::startBoxHeight + 15};

        recordButton->setBounds(startX, currentY, trackButtonsSize, trackButtonsSize);
        recordButton->onClick = [i]() { std::cout << "Recording[" << i + 1 << "]" << std::endl; };

        soloButton->setBounds(startX - xDifference, currentY, trackButtonsSize, trackButtonsSize);
        soloButton->onClick = [this, i, soloButtonPtr = soloButton.get()]()
        {
            const bool currentState = soloButtonPtr->getToggleState();
            soloButtonPtr->setToggleState(!currentState, juce::dontSendNotification);
            trackManager.setPropertyForAllClipsInTrack(i, AudioClipProperty::SOLO, !currentState);
            std::cout << "Soloing[" << i + 1 << "] - " << (!currentState ? "ON" : "OFF") << std::endl;
        };
        muteButton->setBounds(startX - 2 * xDifference, currentY, trackButtonsSize, trackButtonsSize);
        muteButton->onClick = [this, i, muteButtonPtr = muteButton.get()]()
        {
            const bool currentState = muteButtonPtr->getToggleState();
            muteButtonPtr->setToggleState(!currentState, juce::dontSendNotification);
            trackManager.setPropertyForAllClipsInTrack(i, AudioClipProperty::MUTE, !currentState);
            std::cout << "Muting[" << i + 1 << "] - " << (!currentState ? "ON" : "OFF") << std::endl;
        };

        trackButtonsVector.push_back({std::move(recordButton), std::move(soloButton), std::move(muteButton)});
        for(auto& button: trackButtonsVector.back()) { addAndMakeVisible(button.get()); }
    }
}
