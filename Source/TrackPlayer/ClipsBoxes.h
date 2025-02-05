#pragma once

#include <juce_gui_extra/juce_gui_extra.h>

class ClipsBox : public juce::Component
{
public:
    ClipsBox(float x_coord, float y_coord);
    ClipsBox(const ClipsBox&);
    ~ClipsBox() override = default;

    void paint(juce::Graphics& g) override;
    void resized() override;

private:
    float x{};
    float y{};

    const float startBoxWidth{50.0};
    const float boxHeight{40.0};
};