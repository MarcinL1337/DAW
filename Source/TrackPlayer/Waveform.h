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
    static void initStaticData();

    juce::AudioFormatReader* formatReader{nullptr};
    static juce::AudioFormatManager formatManager;
    static std::unique_ptr<juce::AudioThumbnailCache> audioThumbnailCache;
    std::unique_ptr<juce::AudioThumbnail> audioThumbnail;
    inline static bool isStaticDataInitialized{false};

    juce::ValueTree& tree;

    uint16_t currentTrackGuiBoxWidth;
    uint16_t currentTrackGuiBoxHeight{};

    double offsetSeconds{0.0};
    NodeID audioClipID;
};