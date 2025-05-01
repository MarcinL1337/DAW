#pragma once

#include <juce_gui_extra/juce_gui_extra.h>
#include "../Constants.h"
#include "Waveform.h"

class TrackGui final : public juce::Component
{
public:
    explicit TrackGui(uint16_t boxWidth, int numOfSeconds, const juce::ValueTree& parentTree);
    explicit TrackGui(uint16_t boxWidth, int numOfBoxes, const juce::ValueTree& parentTree,
                      const juce::String& newAudioFilePath, const NodeID newAudioClipID);
    TrackGui(const TrackGui&) = delete;
    TrackGui& operator=(const TrackGui&) = delete;
    ~TrackGui() override = default;

    float getBoxWidthToFloat() const { return currentBoxWidth; }
    float getBoxHeightToFloat() const { return currentBoxHeight; }
    uint16_t getBoxWidth() const { return currentBoxWidth; }
    uint16_t getBoxHeight() const { return currentBoxHeight; }
    void changeBoxWidth(uint16_t newBoxWidth);
    void changeBoxHeight(uint16_t newBoxHeight);
    void changeNumOfSeconds(const int numOfSeconds) { currentNumOfSeconds = numOfSeconds; }

    bool hasNoAudioClips() const { return waveforms.empty(); }
    void addNewAudioFile(const juce::String& newAudioFilePath, NodeID newAudioClipID);
    void setOffsetOfWaveformInSeconds(NodeID audioClipID, double offsetSeconds) const;

private:
    void paint(juce::Graphics& g) override;
    void resized() override;

    void makeNewWaveformFromAudioFilePath(const juce::String& newAudioFilePath, NodeID newAudioClipID);

    std::vector<std::unique_ptr<Waveform>> waveforms{};
    juce::ValueTree tree;

    uint16_t currentBoxWidth;
    uint16_t currentBoxHeight{TrackPlayerConstants::startBoxHeight};
    int currentNumOfSeconds{};
};