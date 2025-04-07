#pragma once

#include <juce_gui_extra/juce_gui_extra.h>

class SideMenu final : public juce::Component
{
public:
    SideMenu();
    ~SideMenu() override = default;

    void paint(juce::Graphics& g) override;
    void resized() override;

    void positionSliders();
    void initSliders();

private:
    juce::Slider faderSlider;
    juce::Label faderLabel;
    juce::Slider delaySlider;
    juce::Label delayLabel;
    juce::Slider reverbSlider;
    juce::Label reverbLabel;
    juce::Slider bassBoostSlider;
    juce::Label bassBoostLabel;

    const uint8_t sliderPadding{20u};
    const uint8_t sliderHeight{40u};
};