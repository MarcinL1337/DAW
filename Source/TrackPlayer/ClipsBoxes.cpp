#include "ClipsBoxes.h"

ClipsBox::ClipsBox(const float x_coord, const float y_coord) : x(x_coord), y(y_coord) {}

void ClipsBox::paint(juce::Graphics& g)
{
    const auto parentWidth{getWidth()};
    const auto startAmountOfBoxes{ceil(parentWidth / startBoxWidth)};

    g.setColour(juce::Colours::lightgrey);

    for(int i{0}; i < static_cast<int>(startAmountOfBoxes); i++)
    {
        g.drawRect(i * startBoxWidth, y, startBoxWidth, boxHeight, 0.75);
    }

    std::cout << "ClipsBoxes: " << getLocalBounds().getHeight() << " x " << getLocalBounds().getWidth() << std::endl;
}

void ClipsBox::resized() {}
