#include "SideMenu.h"

SideMenu::SideMenu(const juce::ValueTree& parentTree) : tree{parentTree}
{
    initTrackPropertiesSliders();
    addAndMakeVisible(zoomSlider);
    zoomSlider.addListener(this);
}

void SideMenu::paint(juce::Graphics& g)
{
    g.setColour(juce::Colours::whitesmoke);
    g.drawRect(getLocalBounds());
}

void SideMenu::resized()
{
    positionTrackPropertiesSliders();
    positionZoomSlider();
}

void SideMenu::initTrackPropertiesSliders()
{
    sliderSettings.push_back(SliderSettings{faderSlider, -15, 15, 0.0f, 1, " dB", faderLabel, "Fader"});
    sliderSettings.push_back(SliderSettings{delaySlider, -5000, 5000, 0.0f, 0, " Samples", delayLabel, "Delay"});
    sliderSettings.push_back(SliderSettings{reverbSlider, 0, 100, 0.0f, 2, " %", reverbLabel, "Reverb"});
    sliderSettings.push_back(SliderSettings{bassBoostSlider, 0, 50, 0.0f, 0, " %", bassBoostLabel, "Bass Boost"});

    for(auto& sliderSetting: sliderSettings)
    {
        addAndMakeVisible(sliderSetting.slider);
        sliderSetting.slider.setRange(sliderSetting.minValue, sliderSetting.maxValue);
        sliderSetting.slider.setTextValueSuffix(sliderSetting.textValueSuffix);
        sliderSetting.slider.setValue(sliderSetting.initialValue);
        sliderSetting.slider.setNumDecimalPlacesToDisplay(sliderSetting.valuesDecimalPoint);

        addAndMakeVisible(sliderSetting.sliderLabel);
        sliderSetting.sliderLabel.setText(sliderSetting.labelText, juce::dontSendNotification);
        sliderSetting.sliderLabel.attachToComponent(&sliderSetting.slider, false);
        sliderSetting.sliderLabel.setJustificationType(juce::Justification::centred);
    }
}

void SideMenu::positionTrackPropertiesSliders() const
{
    auto currentY{50u};
    for(auto& sliderSetting: sliderSettings)
    {
        sliderSetting.slider.setSize(getWidth() * 0.95, sliderHeight);
        sliderSetting.slider.setCentrePosition(getWidth() * 0.5, currentY);
        currentY += paddingBetweenSliders;
    }
}

void SideMenu::positionZoomSlider()
{
    zoomSlider.setBounds(0.05 * getWidth(), getHeight() - 1.5 * zoomSliderHeight, 0.9 * getWidth(), zoomSliderHeight);
}

void SideMenu::sliderValueChanged(juce::Slider* changedSlider)
{
    if(changedSlider == &zoomSlider)
    {
        tree.setProperty(trackPlayerZoomPercentage, changedSlider->getValue(), nullptr);
    }
}
