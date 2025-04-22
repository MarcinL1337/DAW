#pragma once

#include <juce_audio_utils/juce_audio_utils.h>
#include <juce_gui_extra/juce_gui_extra.h>
#include "../Constants.h"

class Waveform final : public juce::Component,
                       public juce::ChangeListener
{
public:
    explicit Waveform(uint16_t boxWidth, const juce::ValueTree& parentTree);
    explicit Waveform(const juce::String& newAudioFilePath, uint16_t boxWidth, const juce::ValueTree& parentTree);
    ~Waveform() override = default;

    void changeBoxWidth(uint16_t newBoxWidth);
    void changeBoxHeight(uint16_t newBoxHeight);

private:
    void paint(juce::Graphics& g) override;
    void resized() override;
    void changeListenerCallback(juce::ChangeBroadcaster* source) override;

    juce::AudioBuffer<float> samplesBuffer{};
    juce::AudioFormatReader* formatReader{nullptr};  // change to unique_ptr
    juce::AudioFormatManager formatManager;
    juce::AudioThumbnailCache audioThumbnailCache;
    juce::AudioThumbnail audioThumbnail;

    juce::ValueTree tree;
    juce::Identifier numOfSecondsChanged{"numOfSecondsChanged"};

    uint16_t currentTrackGuiBoxWidth;
    uint16_t currentTrackGuiBoxHeight;
};