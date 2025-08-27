#pragma once

#include <juce_audio_utils/juce_audio_utils.h>
#include <juce_gui_extra/juce_gui_extra.h>
#include "../Constants.h"
#include "../Fade/FadeController.h"

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

    void mouseDrag(const juce::MouseEvent& event) override;
    void mouseDown(const juce::MouseEvent& event) override;
    void mouseMove(const juce::MouseEvent& event) override;
    void mouseEnter(const juce::MouseEvent& event) override;
    void mouseExit(const juce::MouseEvent& event) override;

private:
    void paint(juce::Graphics& g) override;
    void drawWaveformWithFade(juce::Graphics& g, const juce::Rectangle<int>& bounds);
    void resized() override;
    void changeListenerCallback(juce::ChangeBroadcaster* source) override;
    static void initStaticData();
    juce::Image createDragThumbnail() const;

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

    std::unique_ptr<FadeController> fadeController;
};