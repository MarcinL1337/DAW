#include "TrackPlayer.h"

TrackPlayer::TrackPlayer()
{
    addAndMakeVisible(trackPlayerViewport);
    trackPlayerViewport.setScrollBarsShown(true, true);
    trackPlayerViewport.setViewedComponent(&clipsBoxesComponent, false);
    flexBoxInit();
}

void TrackPlayer::paint(juce::Graphics& g)
{
    g.setColour(juce::Colours::lightgrey);
    g.drawRect(getLocalBounds());
}

void TrackPlayer::resized()
{
    trackPlayerViewport.setSize(getWidth(), getHeight());
    clipsBoxesComponent.setSize(getWidth() * 2, getHeight() * 2);
    for(auto i{0}; i < 20; i++)
    {
        clipsBoxesVector.at(i)->setBounds(
            0, i * clipsBoxesVector.at(i)->getGridBoxHeight(), getWidth() * 2, getHeight() * 2);
    }
}

void TrackPlayer::flexBoxInit()
{
    clipsBoxesVector.clear();
    for(auto i{0}; i < 20; i++)
    {
        float x{0.0f};
        float y{0.0f};
        auto clipsBox = std::make_unique<ClipsBox>(x, y);
        clipsBoxesVector.push_back(std::move(clipsBox));
        clipsBoxesComponent.addAndMakeVisible(clipsBoxesVector.back().get());
    }
}
