#include "TrackPlayerSideMenu.h"

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
        soloButton->onClick = [i]() { std::cout << "Soloing[" << i + 1 << "]" << std::endl; };

        muteButton->setBounds(currentTrackButtonsArea.removeFromRight(trackButtonsSize)
                                  .withSizeKeepingCentre(trackButtonsSize, trackButtonsSize)
                                  .reduced(buttonMargin));
        muteButton->onClick = [i]() { std::cout << "Muting[" << i + 1 << "]" << std::endl; };

        trackButtonsVector.push_back({std::move(recordButton), std::move(soloButton), std::move(muteButton)});
        for(auto& button: trackButtonsVector.back()) { addAndMakeVisible(button.get()); }
    }
}
