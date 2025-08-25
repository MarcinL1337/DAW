#pragma once
#include <juce_gui_extra/juce_gui_extra.h>

class ZoomSlider final : public juce::Slider
{
public:
    ZoomSlider();

    double snapValue(const double attemptedValue, DragMode dragMode) override;

private:
    juce::Label zoomSliderLabel;
};