#include "SideMenu.h"
#include "../Constants.h"

SideMenu::SideMenu(juce::ValueTree& parentTree) : tree{parentTree}
{
    initTrackPropertiesSliders();
    initReverbButton();
    initReverbSliders();
    initZoomSlider();
    tree.addListener(this);
}

void SideMenu::paint(juce::Graphics& g)
{
    g.setColour(juce::Colours::whitesmoke);
    g.drawRect(getLocalBounds());
}

void SideMenu::resized()
{
    positionTrackPropertiesSliders();
    positionReverbButtonAndSliders();
    positionZoomSlider();
}

void SideMenu::initReverbButton()
{
    addAndMakeVisible(reverbButton);
    reverbButton.onClick = [this]()
    {
        isReverbOn = !isReverbOn;
        positionReverbButtonAndSliders();
        const juce::Array<juce::var> trackIndexAndNewReverbValue{currentTrackIndex, isReverbOn};
        tree.setProperty(ValueTreeIDs::trackReverbChanged, trackIndexAndNewReverbValue, nullptr);
    };
    addAndMakeVisible(reverbButtonLabel);
    reverbButtonLabel.attachToComponent(&reverbButton, true);
    reverbButtonLabel.setText("Reverb", juce::dontSendNotification);
    reverbButtonLabel.setJustificationType(juce::Justification::centred);
}

void SideMenu::initZoomSlider()
{
    addAndMakeVisible(zoomSlider);
    zoomSlider.addListener(this);
}

void SideMenu::initTrackPropertiesSliders()
{
    sliderSettings.push_back(SliderSettings{gainSlider, -40, 40, 1, " dB", gainLabel, "Gain"});
    sliderSettings.push_back(SliderSettings{panSlider, -1, 1, 1, "", panLabel, "Panner"});

    for(auto& sliderSetting: sliderSettings)
    {
        addAndMakeVisible(sliderSetting.slider);
        sliderSetting.slider.addListener(this);
        sliderSetting.slider.setRange(sliderSetting.minValue, sliderSetting.maxValue);
        sliderSetting.slider.setTextValueSuffix(sliderSetting.textValueSuffix);
        sliderSetting.slider.setNumDecimalPlacesToDisplay(sliderSetting.valuesDecimalPoint);

        addAndMakeVisible(sliderSetting.sliderLabel);
        sliderSetting.sliderLabel.setText(sliderSetting.labelText, juce::dontSendNotification);
        sliderSetting.sliderLabel.attachToComponent(&sliderSetting.slider, false);
        sliderSetting.sliderLabel.setJustificationType(juce::Justification::centred);
    }
}

void SideMenu::initReverbSliders()
{
    reverbSliderSettings.push_back(SliderSettings{roomSizeSlider, 0, 100, 0, " %", roomSizeLabel, "Room size"});
    reverbSliderSettings.push_back(SliderSettings{dampingSlider, 0, 100, 0, " %", dampingLabel, "Damp"});
    reverbSliderSettings.push_back(SliderSettings{wetLevelSlider, 0, 100, 0, " %", wetLevelLabel, "Wet Level"});
    reverbSliderSettings.push_back(SliderSettings{dryLevelSlider, 0, 100, 0, " %", dryLevelLabel, "Dry Level"});
    reverbSliderSettings.push_back(SliderSettings{reverbWidthSlider, 0, 100, 0, " %", reverbWidthLabel, "Width"});
    reverbSliderSettings.push_back(SliderSettings{reverbFreezeSlider, 0, 100, 0, " %", reverbFreezeLabel, "Freeze"});

    for(auto& sliderSetting: reverbSliderSettings)
    {
        addChildComponent(sliderSetting.slider);
        sliderSetting.slider.addListener(this);
        sliderSetting.slider.setSliderStyle(juce::Slider::SliderStyle::Rotary);
        sliderSetting.slider.setRange(sliderSetting.minValue, sliderSetting.maxValue);
        sliderSetting.slider.setTextValueSuffix(sliderSetting.textValueSuffix);
        sliderSetting.slider.setTextBoxStyle(juce::Slider::TextEntryBoxPosition::TextBoxBelow, false, 50, 15);
        sliderSetting.slider.setNumDecimalPlacesToDisplay(sliderSetting.valuesDecimalPoint);

        addChildComponent(sliderSetting.sliderLabel);
        sliderSetting.sliderLabel.setText(sliderSetting.labelText, juce::dontSendNotification);
        sliderSetting.sliderLabel.attachToComponent(&sliderSetting.slider, false);
        sliderSetting.sliderLabel.setJustificationType(juce::Justification::centred);
    }
}

void SideMenu::positionTrackPropertiesSliders() const
{
    uint16_t currentY{50u};
    const auto widthPercentage = sliderValuesPerTrack.empty() ? 0.0 : 0.95;
    for(auto& sliderSetting: sliderSettings)
    {
        sliderSetting.slider.setSize(getWidth() * widthPercentage, sliderHeight);
        sliderSetting.slider.setCentrePosition(getWidth() * 0.5, currentY);
        currentY += paddingBetweenSliders;
    }
}

void SideMenu::positionReverbButtonAndSliders()
{
    auto currentY{panSlider.getY() + paddingBetweenSliders};
    reverbButton.setSize(30, 30);
    reverbButton.setCentrePosition(getWidth() * 0.5, currentY);
    currentY += paddingBetweenSliders;

    // TODO: refactor this
    if(isReverbOn)
    {
        roomSizeSlider.setSize(getWidth() * 0.3, 2 * sliderHeight);
        roomSizeSlider.setCentrePosition(getWidth() * 0.167, currentY);
        roomSizeSlider.setVisible(true);

        dampingSlider.setSize(getWidth() * 0.3, 2 * sliderHeight);
        dampingSlider.setCentrePosition(getWidth() * 0.5, currentY);
        dampingSlider.setVisible(true);

        wetLevelSlider.setSize(getWidth() * 0.3, 2 * sliderHeight);
        wetLevelSlider.setCentrePosition(getWidth() * 0.833, currentY);
        wetLevelSlider.setVisible(true);

        currentY += 1.5 * paddingBetweenSliders;

        dryLevelSlider.setSize(getWidth() * 0.3, 2 * sliderHeight);
        dryLevelSlider.setCentrePosition(getWidth() * 0.167, currentY);
        dryLevelSlider.setVisible(true);

        reverbWidthSlider.setSize(getWidth() * 0.3, 2 * sliderHeight);
        reverbWidthSlider.setCentrePosition(getWidth() * 0.5, currentY);
        reverbWidthSlider.setVisible(true);

        reverbFreezeSlider.setSize(getWidth() * 0.3, 2 * sliderHeight);
        reverbFreezeSlider.setCentrePosition(getWidth() * 0.833, currentY);
        reverbFreezeSlider.setVisible(true);
    }
    else
    {
        for(const auto& sliderSetting: reverbSliderSettings) { sliderSetting.slider.setVisible(false); }
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
        const juce::Array<juce::var> trackIndexAndNewSliderValue{currentTrackIndex, gainSlider.getValue()};
        tree.setProperty(ValueTreeIDs::trackGainChanged, trackIndexAndNewSliderValue, nullptr);
    }
    else if(changedSlider == &panSlider)
    {
        sliderValuesPerTrack.at(currentTrackIndex).panValue = panSlider.getValue();
        const juce::Array<juce::var> trackIndexAndNewSliderValue{currentTrackIndex, panSlider.getValue()};
        tree.setProperty(ValueTreeIDs::trackPanChanged, trackIndexAndNewSliderValue, nullptr);
    }
    else if(changedSlider == &roomSizeSlider)
    {
        sliderValuesPerTrack.at(currentTrackIndex).roomSizeValue = roomSizeSlider.getValue();
        const juce::Array<juce::var> trackIndexAndNewSliderValue{currentTrackIndex, roomSizeSlider.getValue()};
        tree.setProperty(ValueTreeIDs::trackRoomSizeChanged, trackIndexAndNewSliderValue, nullptr);
    }
    else if(changedSlider == &dampingSlider)
    {
        sliderValuesPerTrack.at(currentTrackIndex).dampingValue = dampingSlider.getValue();
        const juce::Array<juce::var> trackIndexAndNewSliderValue{currentTrackIndex, dampingSlider.getValue()};
        tree.setProperty(ValueTreeIDs::trackDampChanged, trackIndexAndNewSliderValue, nullptr);
    }
    else if(changedSlider == &wetLevelSlider)
    {
        sliderValuesPerTrack.at(currentTrackIndex).wetLevelValue = wetLevelSlider.getValue();
        const juce::Array<juce::var> trackIndexAndNewSliderValue{currentTrackIndex, wetLevelSlider.getValue()};
        tree.setProperty(ValueTreeIDs::trackWetLevelChanged, trackIndexAndNewSliderValue, nullptr);
    }
    else if(changedSlider == &dryLevelSlider)
    {
        sliderValuesPerTrack.at(currentTrackIndex).dryLevelValue = dryLevelSlider.getValue();
        const juce::Array<juce::var> trackIndexAndNewSliderValue{currentTrackIndex, dryLevelSlider.getValue()};
        tree.setProperty(ValueTreeIDs::trackDryLevelChanged, trackIndexAndNewSliderValue, nullptr);
    }
    else if(changedSlider == &reverbWidthSlider)
    {
        sliderValuesPerTrack.at(currentTrackIndex).reverbWidthValue = reverbWidthSlider.getValue();
        const juce::Array<juce::var> trackIndexAndNewSliderValue{currentTrackIndex, reverbWidthSlider.getValue()};
        tree.setProperty(ValueTreeIDs::trackWidthChanged, trackIndexAndNewSliderValue, nullptr);
    }
    else if(changedSlider == &reverbFreezeSlider)
    {
        sliderValuesPerTrack.at(currentTrackIndex).reverbFreezeValue = reverbFreezeSlider.getValue();
        const juce::Array<juce::var> trackIndexAndNewSliderValue{currentTrackIndex, reverbFreezeSlider.getValue()};
        tree.setProperty(ValueTreeIDs::trackFreezeChanged, trackIndexAndNewSliderValue, nullptr);
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
        auto& [newGainValue,
               newFaderValue,
               newRoomSizeValue,
               newDampValue,
               newWetLevelValue,
               newDryLevelValue,
               newWidthValue,
               newFreezeValue] = sliderValuesPerTrack.at(currentTrackIndex);

        gainSlider.setValue(newGainValue);
        panSlider.setValue(newFaderValue);
        roomSizeSlider.setValue(newRoomSizeValue);
        dampingSlider.setValue(newDampValue);
        wetLevelSlider.setValue(newWetLevelValue);
        dryLevelSlider.setValue(newDryLevelValue);
        reverbWidthSlider.setValue(newWidthValue);
        reverbFreezeSlider.setValue(newFreezeValue);
    }
}

void SideMenu::addTrack()
{
    sliderValuesPerTrack.push_back(SliderValues{0.0f, 0.0f, 50, 50, 33, 40, 100, 0});
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
