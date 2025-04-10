#pragma once

#include <juce_gui_extra/juce_gui_extra.h>

struct SliderSettings
{
    juce::Slider& slider;
    int minValue;
    int maxValue;
    float initialValue;
    int valuesDecimalPoint;
    std::string textValueSuffix;
    juce::Label& sliderLabel;
    std::string labelText;
};

class SideMenu final : public juce::Component
{
public:
    SideMenu();
    ~SideMenu() override = default;

    void paint(juce::Graphics& g) override;
    void resized() override;

    void positionSliders() const;
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

    std::vector<SliderSettings> sliderSettings{};

    const uint8_t paddingBetweenSliders{100u};
    const uint8_t sliderHeight{40u};
};