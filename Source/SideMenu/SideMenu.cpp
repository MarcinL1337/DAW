#include "SideMenu.h"
#include "../Constants.h"

SideMenu::SideMenu(juce::ValueTree& parentTree) : tree{parentTree}
{
    initTrackPropertiesSliders();
    addAndMakeVisible(zoomSlider);
    tree.addListener(this);
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
    sliderSettings.push_back(SliderSettings{gainSlider, -40, 40, 0.0f, 1, " dB", gainLabel, "Gain"});
    sliderSettings.push_back(SliderSettings{faderSlider, -15, 15, 0.0f, 1, " dB", faderLabel, "Fader"});
    sliderSettings.push_back(SliderSettings{delaySlider, -5000, 5000, 0.0f, 0, " Samples", delayLabel, "Delay"});
    sliderSettings.push_back(SliderSettings{reverbSlider, 0, 100, 0.0f, 2, " %", reverbLabel, "Reverb"});
    sliderSettings.push_back(SliderSettings{bassBoostSlider, 0, 50, 0.0f, 0, " %", bassBoostLabel, "Bass Boost"});

    for(auto& sliderSetting: sliderSettings)
    {
        addAndMakeVisible(sliderSetting.slider);
        sliderSetting.slider.addListener(this);
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
    auto widthPercentage = sliderValuesPerTrack.size() > 0 ? 0.95 : 0.0;
    for(auto& sliderSetting: sliderSettings)
    {
        sliderSetting.slider.setSize(getWidth() * widthPercentage, sliderHeight);
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
        tree.setProperty(ValueTreeIDs::trackPlayerZoomPercentage, changedSlider->getValue(), nullptr);
    }
    else if(changedSlider == &gainSlider)
    {
        sliderValuesPerTrack.at(currentTrackIndex).gainValue = gainSlider.getValue();
        const juce::Array<juce::var> trackAndGainInfo{currentTrackIndex, gainSlider.getValue()};
        tree.setProperty(ValueTreeIDs::trackGainChanged, trackAndGainInfo, nullptr);
    }
    else if(changedSlider == &faderSlider)
    {
        sliderValuesPerTrack.at(currentTrackIndex).faderValue = faderSlider.getValue();
    }
    else if(changedSlider == &delaySlider)
    {
        sliderValuesPerTrack.at(currentTrackIndex).delayValue = delaySlider.getValue();
    }
    else if(changedSlider == &reverbSlider)
    {
        sliderValuesPerTrack.at(currentTrackIndex).reverbValue = reverbSlider.getValue();
    }
    else if(changedSlider == &bassBoostSlider)
    {
        sliderValuesPerTrack.at(currentTrackIndex).bassBoostValue = bassBoostSlider.getValue();
    }
}

void SideMenu::chooseNewTrackToBeSelected(const int deletedTrackIndex)
{
    if(not sliderValuesPerTrack.empty())
    {
        if(deletedTrackIndex < currentTrackIndex)
        {
            currentTrackIndex--;
        }
        else if(deletedTrackIndex == currentTrackIndex)
        {
            currentTrackIndex = deletedTrackIndex % sliderValuesPerTrack.size();
        }
    }
    else
    {
        currentTrackIndex = TrackPlayerConstants::noTrackChosen;
        resized();
    }
    tree.setProperty(ValueTreeIDs::setSelectedTrack, currentTrackIndex, nullptr);
}

void SideMenu::valueTreePropertyChanged(juce::ValueTree&, const juce::Identifier& property)
{
    if(static_cast<int>(tree[property]) == ValueTreeConstants::doNothing)
    {
        return;
    }
    if(property == ValueTreeIDs::setSelectedTrack)
    {
        currentTrackIndex = tree[ValueTreeIDs::setSelectedTrack];
        displaySliderValuesForCurrentTrack();
    }
    else if(property == ValueTreeIDs::duplicateTrackGui)
    {
        currentTrackIndex = sliderValuesPerTrack.size() - 1;
        displaySliderValuesForCurrentTrack();
        tree.setProperty(ValueTreeIDs::setSelectedTrack, currentTrackIndex, nullptr);
    }
    else if(property == ValueTreeIDs::reorderTracks)
    {
        const int fromIndex = tree[ValueTreeIDs::reorderTracks][0];
        const int toIndex = tree[ValueTreeIDs::reorderTracks][1];
        reorderSliderValues(fromIndex, toIndex);
    }
}

void SideMenu::displaySliderValuesForCurrentTrack()
{
    if(currentTrackIndex != TrackPlayerConstants::noTrackChosen)
    {
        auto& [newGainValue, newFaderValue, newDelayValue, newReverbValue, newBassBoostValue] =
            sliderValuesPerTrack.at(currentTrackIndex);

        gainSlider.setValue(newGainValue);
        faderSlider.setValue(newFaderValue);
        delaySlider.setValue(newDelayValue);
        reverbSlider.setValue(newReverbValue);
        bassBoostSlider.setValue(newBassBoostValue);
    }
}

void SideMenu::addTrack()
{
    sliderValuesPerTrack.push_back(SliderValues{0.0f, 0.0f, 0.0f, 0.0f, 0.0f});
    currentTrackIndex = sliderValuesPerTrack.size() - 1;
    displaySliderValuesForCurrentTrack();
    tree.setProperty(ValueTreeIDs::setSelectedTrack, currentTrackIndex, nullptr);
    resized();
}

void SideMenu::removeTrack(const int trackToBeDeletedIndex)
{
    sliderValuesPerTrack.erase(sliderValuesPerTrack.begin() + trackToBeDeletedIndex);
    chooseNewTrackToBeSelected(trackToBeDeletedIndex);
}

void SideMenu::setTrackProperties(const int trackIndex, const float gainValue)
{
    sliderValuesPerTrack.at(trackIndex).gainValue = gainValue;
}

void SideMenu::reorderSliderValues(const int fromIndex, const int toIndex)
{
    const auto sliderValuesToMove = std::move(sliderValuesPerTrack[fromIndex]);
    sliderValuesPerTrack.erase(sliderValuesPerTrack.begin() + fromIndex);
    sliderValuesPerTrack.insert(sliderValuesPerTrack.begin() + toIndex, sliderValuesToMove);

    currentTrackIndex = toIndex;
    displaySliderValuesForCurrentTrack();
}
