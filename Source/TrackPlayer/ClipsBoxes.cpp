#include "ClipsBoxes.h"

ClipsBox::ClipsBox(const float x_coord, const float y_coord) : x(x_coord), y(y_coord) {}

ClipsBox::ClipsBox(const ClipsBox& other) : x(other.x), y(other.y)
{
    std::cout << "ClipsBox(): x = " << x << ", y = " << y << std::endl;
}

void ClipsBox::paint(juce::Graphics& g)
{
    const auto parentWidth{getWidth()};
    const auto startAmountOfBoxes{ceil(parentWidth / startBoxWidth)};

    g.setColour(juce::Colours::lightgrey);

    for(int i{0}; i < static_cast<int>(startAmountOfBoxes); i++) { g.drawRect(x, y * i, startBoxWidth, boxHeight); }
}

void ClipsBox::resized() {}
