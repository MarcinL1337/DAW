#include "TrackPlayer.h"

TrackPlayer::TrackPlayer() { flexBoxInit(); }

void TrackPlayer::paint(juce::Graphics& g)
{
    g.setColour(juce::Colours::lightgrey);
    g.drawRect(getLocalBounds());
    std::cout << "TrackPlayer: " << getLocalBounds().getHeight() << " x " << getLocalBounds().getWidth() << std::endl;
}

void TrackPlayer::resized()
{
    for(auto i{0}; i < 30; i++)
    {
        clipsBoxesVector.at(i)->setBounds(0, i * clipsBoxesVector.at(i)->getClipBoxWidth(), getWidth(), getHeight());
    }
}

void TrackPlayer::flexBoxInit()
{
    clipsBoxesVector.clear();
    for(auto i{0}; i < 30; i++)
    {
        float x{0.0f};
        float y{0.0f};
        auto clipsBox = std::make_unique<ClipsBox>(x, y);
        clipsBoxesVector.push_back(std::move(clipsBox));
        addAndMakeVisible(clipsBoxesVector.back().get());
    }
}
