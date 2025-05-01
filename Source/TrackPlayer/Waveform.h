#pragma once

#include <juce_audio_utils/juce_audio_utils.h>
#include <juce_gui_extra/juce_gui_extra.h>
#include "../Constants.h"

using NodeID = juce::AudioProcessorGraph::NodeID;

class Waveform final : public juce::Component,
                       public juce::ChangeListener
{
public:
    explicit Waveform(uint16_t boxWidth, juce::ValueTree& parentTree, NodeID newAudioClipID);
    explicit Waveform(const juce::String& newAudioFilePath, uint16_t boxWidth, juce::ValueTree& parentTree,
                      NodeID newAudioClipID);
    ~Waveform() override = default;

    void changeBoxWidth(uint16_t newBoxWidth);
    void changeBoxHeight(uint16_t newBoxHeight);

    NodeID getAudioClipID() const { return audioClipID; }
    void setOffsetSeconds(double newOffsetSeconds);
    double getOffsetSeconds() const { return offsetSeconds; }

private:
    void paint(juce::Graphics& g) override;
    void resized() override;
    void changeListenerCallback(juce::ChangeBroadcaster* source) override;

    // TODO: when TrackManager will be ready, think if this is necessary here
    juce::AudioBuffer<float> samplesBuffer{};
    juce::AudioFormatReader* formatReader{nullptr};  // change to unique_ptr -> for now it stays as is
    juce::AudioFormatManager formatManager;
    juce::AudioThumbnailCache audioThumbnailCache;
    juce::AudioThumbnail audioThumbnail;

    juce::ValueTree& tree;
    juce::Identifier numOfSecondsChanged{"numOfSecondsChanged"};

    uint16_t currentTrackGuiBoxWidth;
    uint16_t currentTrackGuiBoxHeight{};

    double offsetSeconds{0.0};
    NodeID audioClipID;
};