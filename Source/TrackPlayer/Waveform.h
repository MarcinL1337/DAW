#pragma once

#include <juce_audio_utils/juce_audio_utils.h>
#include <juce_gui_extra/juce_gui_extra.h>
#include "../Audio/Fade.h"
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

    void setFadeInLength(float lengthInSeconds);
    void setFadeOutLength(float lengthInSeconds);
    float getFadeInLength() const { return fadeInData.lengthSeconds; }
    float getFadeOutLength() const { return fadeOutData.lengthSeconds; }
    void setFadeInType(FadeType type);
    void setFadeOutType(FadeType type);
    FadeType getFadeInType() const { return fadeInData.type; }
    FadeType getFadeOutType() const { return fadeOutData.type; }

private:
    void paint(juce::Graphics& g) override;
    void resized() override;
    void changeListenerCallback(juce::ChangeBroadcaster* source) override;
    void mouseDown(const juce::MouseEvent& event) override;
    void mouseDrag(const juce::MouseEvent& event) override;
    void mouseUp(const juce::MouseEvent& event) override;
    void mouseMove(const juce::MouseEvent& event) override;

    void showFadeTypeMenu(bool isFadeIn);
    void drawFadeHandles(juce::Graphics& g);
    void drawFadeShape(juce::Graphics& g);
    bool isMouseOverFadeInHandle(const juce::Point<int>& position) const;
    bool isMouseOverFadeOutHandle(const juce::Point<int>& position) const;
    void updateFadeAfterDrag(bool isFadeIn, float newLength);
    void updateFadesInAudioProcessor();

    // TODO: when TrackManager will be ready, think if formatReader and formatManager are necessary here
    juce::AudioBuffer<float> samplesBuffer{};
    juce::AudioFormatReader* formatReader{nullptr};  // change to unique_ptr -> for now it stays as is
    juce::AudioFormatManager formatManager;
    juce::AudioThumbnailCache audioThumbnailCache;
    juce::AudioThumbnail audioThumbnail;

    juce::ValueTree& tree;

    uint16_t currentTrackGuiBoxWidth;
    uint16_t currentTrackGuiBoxHeight{};

    double offsetSeconds{0.0};
    NodeID audioClipID;

    FadeData fadeInData{true, 0.0f, FadeType::Linear};
    FadeData fadeOutData{true, 0.0f, FadeType::Linear};
    const int handleSize{10};
    bool draggingFadeIn{false};
    bool draggingFadeOut{false};
    bool mouseOverFadeInHandle{false};
    bool mouseOverFadeOutHandle{false};
};