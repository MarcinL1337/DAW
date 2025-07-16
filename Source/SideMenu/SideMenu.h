#pragma once

#include <juce_gui_extra/juce_gui_extra.h>
#include "ZoomSlider.h"

struct SliderSettings
{
    juce::Slider& slider;
    int minValue;
    int maxValue;
    int valuesDecimalPoint;
    std::string textValueSuffix;
    juce::Label& sliderLabel;
    std::string labelText;
};

struct SliderValues
{
    double gainValue;
    double panValue;
    double roomSizeValue;
    double dampingValue;
    double wetLevelValue;
    double dryLevelValue;
    double reverbWidthValue;
    double reverbFreezeValue;
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
    void positionReverbButtonAndSliders();
    void initTrackPropertiesSliders();
    void initReverbSliders();
    void initReverbButton();
    void initZoomSlider();
    void initReverbFlexItems();
    void initRunTimeVariables();

    void positionZoomSlider();
    void displaySliderValuesForCurrentTrack();
    void chooseNewTrackToBeSelected(const int deletedTrackIndex);
    void reorderSliderValues(int fromIndex, int toIndex);

    juce::Slider gainSlider;
    juce::Label gainLabel;
    juce::Slider panSlider;
    juce::Label panLabel;
    juce::Slider roomSizeSlider;
    juce::Label roomSizeLabel;
    juce::Slider dampingSlider;
    juce::Label dampingLabel;
    juce::Slider wetLevelSlider;
    juce::Label wetLevelLabel;
    juce::Slider dryLevelSlider;
    juce::Label dryLevelLabel;
    juce::Slider reverbWidthSlider;
    juce::Label reverbWidthLabel;
    juce::Slider reverbFreezeSlider;
    juce::Label reverbFreezeLabel;

    juce::ToggleButton reverbButton;
    juce::Label reverbButtonLabel;
    bool isReverbOn{false};

    std::vector<SliderSettings> reverbSliderSettings{};
    std::vector<SliderSettings> sliderSettings{};
    std::vector<SliderValues> sliderValuesPerTrack{};

    int currentTrackIndex{0};
    uint8_t paddingBetweenSliders{};
    uint8_t sliderHeight{};

    ZoomSlider zoomSlider{};
    int zoomSliderHeight{};

    juce::ValueTree& tree;

    int reverbSlidersFlexBoxHeight{};
    inline static bool areReverbFlexItemsInitialized{false};
    juce::FlexBox reverbSliders{juce::FlexBox::Direction::column,
                                juce::FlexBox::Wrap::noWrap,
                                juce::FlexBox::AlignContent::spaceBetween,
                                juce::FlexBox::AlignItems::stretch,
                                juce::FlexBox::JustifyContent::spaceBetween};
    juce::FlexBox reverbSlidersFirstRow{juce::FlexBox::Direction::row,
                                        juce::FlexBox::Wrap::noWrap,
                                        juce::FlexBox::AlignContent::spaceBetween,
                                        juce::FlexBox::AlignItems::stretch,
                                        juce::FlexBox::JustifyContent::spaceBetween};
    juce::FlexBox reverbSlidersSecondRow{juce::FlexBox::Direction::row,
                                         juce::FlexBox::Wrap::noWrap,
                                         juce::FlexBox::AlignContent::spaceBetween,
                                         juce::FlexBox::AlignItems::stretch,
                                         juce::FlexBox::JustifyContent::spaceBetween};
};