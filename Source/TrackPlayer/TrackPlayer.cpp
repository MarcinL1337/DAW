#include "TrackPlayer.h"

TrackPlayer::TrackPlayer()
{
    flexBoxInit();
    for(auto& box: clipsBoxesArray) { addAndMakeVisible(box); }
}

void TrackPlayer::paint(juce::Graphics& g)
{
    g.setColour(juce::Colours::lightgrey);
    g.drawRect(getLocalBounds());
    std::cout << "TrackPlayer: " << getLocalBounds().getHeight() << " x " << getLocalBounds().getWidth() << std::endl;
}

void TrackPlayer::resized() {}

void TrackPlayer::flexBoxInit()
{
    for(auto i{0}; i < 10; i++)
    {
        ClipsBox clipsBox{0.0, static_cast<float>(50.0 * (i + 1))};
        clipsBoxesArray.add(clipsBox);
        clipsBoxes.items.add(juce::FlexItem(static_cast<float>(getWidth()), 50.0, clipsBox));
    }
}
