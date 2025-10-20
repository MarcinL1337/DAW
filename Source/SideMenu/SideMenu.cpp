#include "SideMenu.h"
#include "../Constants.h"

SideMenu::SideMenu(juce::ValueTree& parentTree) : tree{parentTree}
{
    initTrackPropertiesSliders();
    initReverbButton();
    initReverbSliders();
    initZoomSlider();
    tree.addListener(this);
    juce::Timer::callAfterDelay(1,
                                [this]()
                                {
                                    initReverbFlexItems();
                                    initRunTimeVariables();
                                });
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

void SideMenu::initRunTimeVariables()
{
    paddingBetweenSliders = static_cast<uint8_t>(getHeight() / 13);
    sliderHeight = static_cast<uint8_t>(paddingBetweenSliders / 2.5f);
    zoomSliderHeight = sliderHeight * 1.5f;
    resized();
}

void SideMenu::initReverbButton()
{
    addChildComponent(reverbButton);
    reverbButton.onClick = [this]()
    {
        toggleReverbStateInCurrentTrack();
        positionReverbButtonAndSliders();
        const juce::Array<juce::var> trackIndexAndNewReverbValue{currentTrackIndex, isReverbOnInCurrentTrack()};
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
    sliderSettings.push_back(SliderSettings{gainSlider, -75, 25, 1, " dB", gainLabel, "Gain"});
    sliderSettings.push_back(SliderSettings{panSlider, -1, 1, 2, "", panLabel, "Panner"});

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
    const auto widthPercentage = controlsValuesPerTrack.empty() ? 0.0 : 0.95;
    for(auto& sliderSetting: sliderSettings)
    {
        sliderSetting.slider.setSize(getWidth() * widthPercentage, sliderHeight);
        sliderSetting.slider.setCentrePosition(getWidth() * 0.5, currentY);
        currentY += paddingBetweenSliders;
    }
}

void SideMenu::initReverbFlexItems()
{
    if(not areReverbFlexItemsInitialized)
    {
        const juce::FlexItem roomSizeItem{roomSizeSlider};
        const juce::FlexItem dampingItem{dampingSlider};
        const juce::FlexItem wetLevelItem{wetLevelSlider};
        reverbSlidersFirstRow.items.add(roomSizeItem.withWidth(getWidth() * 0.3f).withMinHeight(100));
        reverbSlidersFirstRow.items.add(dampingItem.withWidth(getWidth() * 0.3f).withMinHeight(100));
        reverbSlidersFirstRow.items.add(wetLevelItem.withWidth(getWidth() * 0.3f).withMinHeight(100));

        const juce::FlexItem dryLevelItem{dryLevelSlider};
        const juce::FlexItem reverbWidthItem{reverbWidthSlider};
        const juce::FlexItem reverbFreezeItem{reverbFreezeSlider};
        reverbSlidersSecondRow.items.add(dryLevelItem.withWidth(getWidth() * 0.3f).withMinHeight(100));
        reverbSlidersSecondRow.items.add(reverbWidthItem.withWidth(getWidth() * 0.3f).withMinHeight(100));
        reverbSlidersSecondRow.items.add(reverbFreezeItem.withWidth(getWidth() * 0.3f).withMinHeight(100));

        reverbSliders.items.add(reverbSlidersFirstRow);
        reverbSliders.items.add(reverbSlidersSecondRow);
        reverbSlidersFlexBoxHeight = static_cast<int>(0.14 * getHeight());
        areReverbFlexItemsInitialized = true;
    }
}

void SideMenu::positionReverbButtonAndSliders()
{
    const auto currentY{panSlider.getY() + paddingBetweenSliders};
    reverbButton.setSize(30, 30);
    reverbButton.setCentrePosition(getWidth() * 0.5, currentY);

    if(currentTrackIndex != TrackPlayerConstants::noTrackChosen and isReverbOnInCurrentTrack())
    {
        for(const auto& sliderSetting: reverbSliderSettings) { sliderSetting.slider.setVisible(true); }
        const juce::Rectangle area{
            0, reverbButton.getY() + reverbButton.getHeight() * 2, getWidth(), reverbSlidersFlexBoxHeight};
        reverbSliders.performLayout(area);
    }
    else
    {
        for(const auto& sliderSetting: reverbSliderSettings) { sliderSetting.slider.setVisible(false); }
        const juce::Rectangle emptyArea{0, 0, 0, 0};
        reverbSliders.performLayout(emptyArea);
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
        controlsValuesPerTrack.at(currentTrackIndex).gainValue = gainSlider.getValue();
        const juce::Array<juce::var> trackIndexAndNewSliderValue{currentTrackIndex, gainSlider.getValue()};
        tree.setProperty(ValueTreeIDs::trackGainChanged, trackIndexAndNewSliderValue, nullptr);
    }
    else if(changedSlider == &panSlider)
    {
        panSlider.setTextValueSuffix(panSlider.getValue() < 0 ? " L" : " R");
        controlsValuesPerTrack.at(currentTrackIndex).panValue = panSlider.getValue();
        const juce::Array<juce::var> trackIndexAndNewSliderValue{currentTrackIndex, panSlider.getValue()};
        tree.setProperty(ValueTreeIDs::trackPanChanged, trackIndexAndNewSliderValue, nullptr);
    }
    else if(changedSlider == &roomSizeSlider)
    {
        controlsValuesPerTrack.at(currentTrackIndex).roomSizeValue = roomSizeSlider.getValue();
        const juce::Array<juce::var> trackIndexAndNewSliderValue{currentTrackIndex, roomSizeSlider.getValue()};
        tree.setProperty(ValueTreeIDs::trackRoomSizeChanged, trackIndexAndNewSliderValue, nullptr);
    }
    else if(changedSlider == &dampingSlider)
    {
        controlsValuesPerTrack.at(currentTrackIndex).dampingValue = dampingSlider.getValue();
        const juce::Array<juce::var> trackIndexAndNewSliderValue{currentTrackIndex, dampingSlider.getValue()};
        tree.setProperty(ValueTreeIDs::trackDampChanged, trackIndexAndNewSliderValue, nullptr);
    }
    else if(changedSlider == &wetLevelSlider)
    {
        controlsValuesPerTrack.at(currentTrackIndex).wetLevelValue = wetLevelSlider.getValue();
        const juce::Array<juce::var> trackIndexAndNewSliderValue{currentTrackIndex, wetLevelSlider.getValue()};
        tree.setProperty(ValueTreeIDs::trackWetLevelChanged, trackIndexAndNewSliderValue, nullptr);
    }
    else if(changedSlider == &dryLevelSlider)
    {
        controlsValuesPerTrack.at(currentTrackIndex).dryLevelValue = dryLevelSlider.getValue();
        const juce::Array<juce::var> trackIndexAndNewSliderValue{currentTrackIndex, dryLevelSlider.getValue()};
        tree.setProperty(ValueTreeIDs::trackDryLevelChanged, trackIndexAndNewSliderValue, nullptr);
    }
    else if(changedSlider == &reverbWidthSlider)
    {
        controlsValuesPerTrack.at(currentTrackIndex).reverbWidthValue = reverbWidthSlider.getValue();
        const juce::Array<juce::var> trackIndexAndNewSliderValue{currentTrackIndex, reverbWidthSlider.getValue()};
        tree.setProperty(ValueTreeIDs::trackWidthChanged, trackIndexAndNewSliderValue, nullptr);
    }
    else if(changedSlider == &reverbFreezeSlider)
    {
        controlsValuesPerTrack.at(currentTrackIndex).reverbFreezeValue = reverbFreezeSlider.getValue();
        const juce::Array<juce::var> trackIndexAndNewSliderValue{currentTrackIndex, reverbFreezeSlider.getValue()};
        tree.setProperty(ValueTreeIDs::trackFreezeChanged, trackIndexAndNewSliderValue, nullptr);
    }
}

void SideMenu::chooseNewTrackToBeSelected(const int deletedTrackIndex)
{
    if(not controlsValuesPerTrack.empty())
    {
        if(deletedTrackIndex < currentTrackIndex)
        {
            currentTrackIndex--;
        }
        else if(deletedTrackIndex == currentTrackIndex)
        {
            currentTrackIndex = deletedTrackIndex % controlsValuesPerTrack.size();
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
        currentTrackIndex = controlsValuesPerTrack.size() - 1;
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
    const bool isVisible = currentTrackIndex != TrackPlayerConstants::noTrackChosen;
    if(isVisible)
    {
        auto& [newGainValue,
               newFaderValue,
               newReverbState,
               newRoomSizeValue,
               newDampValue,
               newWetLevelValue,
               newDryLevelValue,
               newWidthValue,
               newFreezeValue] = controlsValuesPerTrack.at(currentTrackIndex);

        gainSlider.setValue(newGainValue, juce::dontSendNotification);
        panSlider.setValue(newFaderValue, juce::dontSendNotification);
        reverbButton.setToggleState(newReverbState, juce::dontSendNotification);
        roomSizeSlider.setValue(newRoomSizeValue, juce::dontSendNotification);
        dampingSlider.setValue(newDampValue, juce::dontSendNotification);
        wetLevelSlider.setValue(newWetLevelValue, juce::dontSendNotification);
        dryLevelSlider.setValue(newDryLevelValue, juce::dontSendNotification);
        reverbWidthSlider.setValue(newWidthValue, juce::dontSendNotification);
        reverbFreezeSlider.setValue(newFreezeValue, juce::dontSendNotification);

        positionReverbButtonAndSliders();
    }
    for(auto& sliderSetting: sliderSettings)
    {
        sliderSetting.slider.setVisible(isVisible);
        sliderSetting.sliderLabel.setVisible(isVisible);
    }
    reverbButton.setVisible(isVisible);
    reverbButtonLabel.setVisible(isVisible);
    for(const auto& sliderSetting: reverbSliderSettings) { sliderSetting.slider.setVisible(isVisible); }
}

void SideMenu::addTrack()
{
    controlsValuesPerTrack.push_back(ControlsValues{0.0f, 0.0f, false, 50, 50, 33, 40, 100, 0});
    currentTrackIndex = controlsValuesPerTrack.size() - 1;
    displaySliderValuesForCurrentTrack();
    tree.setProperty(ValueTreeIDs::setSelectedTrack, currentTrackIndex, nullptr);
    resized();
}

void SideMenu::removeTrack(const int trackToBeDeletedIndex)
{
    controlsValuesPerTrack.erase(controlsValuesPerTrack.begin() + trackToBeDeletedIndex);
    chooseNewTrackToBeSelected(trackToBeDeletedIndex);
}

void SideMenu::setTrackProperties(const int trackIndex, const float gainValue)
{
    controlsValuesPerTrack.at(trackIndex).gainValue = gainValue;
    displaySliderValuesForCurrentTrack();  // TODO: check if needed here
}

void SideMenu::reorderSliderValues(const int fromIndex, const int toIndex)
{
    const auto sliderValuesToMove = std::move(controlsValuesPerTrack[fromIndex]);
    controlsValuesPerTrack.erase(controlsValuesPerTrack.begin() + fromIndex);
    controlsValuesPerTrack.insert(controlsValuesPerTrack.begin() + toIndex, sliderValuesToMove);

    currentTrackIndex = toIndex;
    displaySliderValuesForCurrentTrack();
}

void SideMenu::clearAllTracks()
{
    displaySliderValuesForCurrentTrack();
    controlsValuesPerTrack.clear();
    resized();
}
