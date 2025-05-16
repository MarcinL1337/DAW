#pragma once

#include <juce_gui_extra/juce_gui_extra.h>
#include "ZoomSlider.h"

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

struct SliderValues
{
    double gainValue;
    double faderValue;
    double delayValue;
    double reverbValue;
    double bassBoostValue;
};

class SideMenu final : public juce::Component,
                       public juce::Slider::Listener,
                       public juce::ValueTree::Listener
{
public:
    explicit SideMenu(juce::ValueTree& parentTree);
    ~SideMenu() override = default;

    void addTrack();
    void removeTrack(const int trackToBeDeletedIndex);
    void setTrackProperties(const int trackIndex, const float gainValue);

private:
    void paint(juce::Graphics& g) override;
    void resized() override;
    void sliderValueChanged(juce::Slider*) override;
    void valueTreePropertyChanged(juce::ValueTree&, const juce::Identifier& property) override;

    void positionTrackPropertiesSliders() const;
    void initTrackPropertiesSliders();

    void positionZoomSlider();
    void displaySliderValuesForCurrentTrack();
    void chooseNewTrackToBeSelected(const int deletedTrackIndex);

    juce::Slider gainSlider;
    juce::Label gainLabel;
    juce::Slider faderSlider;
    juce::Label faderLabel;
    juce::Slider delaySlider;
    juce::Label delayLabel;
    juce::Slider reverbSlider;
    juce::Label reverbLabel;
    juce::Slider bassBoostSlider;
    juce::Label bassBoostLabel;

    std::vector<SliderSettings> sliderSettings{};
    std::vector<SliderValues> sliderValuesPerTrack{};

    int currentTrackIndex{0};
    const uint8_t paddingBetweenSliders{100u};
    const uint8_t sliderHeight{40u};

    ZoomSlider zoomSlider{};
    const int zoomSliderHeight{60};

    juce::ValueTree& tree;
};