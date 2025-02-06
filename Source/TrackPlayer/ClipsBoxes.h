#pragma once

#include <juce_gui_extra/juce_gui_extra.h>

class ClipsBox : public juce::Component
{
public:
    ClipsBox(float x_coord, float y_coord);
    ClipsBox(const ClipsBox&) = delete;
    ClipsBox& operator=(const ClipsBox&) = delete;
    ~ClipsBox() override = default;

    void paint(juce::Graphics& g) override;
    void resized() override;

    float getClipBoxHeight() const { return startBoxWidth; }
    float getClipBoxWidth() const { return boxHeight; }

private:
    float x{};
    float y{};

    const float startBoxWidth{70.0};
    const float boxHeight{85.0};
};