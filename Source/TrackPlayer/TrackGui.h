#pragma once

#include <juce_gui_extra/juce_gui_extra.h>
#include "../Constants.h"
#include "Waveform.h"

class TrackGuiManager;

class TrackGui final : public juce::Component
{
public:
    explicit TrackGui(uint16_t boxWidth, int numOfSeconds, juce::ValueTree& parentTree);

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
    void mouseDown(const juce::MouseEvent& event) override;

    void makeNewWaveformFromAudioFilePath(const juce::String& newAudioFilePath, NodeID newAudioClipID);
    void showPopUpMenuForTrack();
    void initPopUpMenuForTrack();
    void showPopUpMenuForClip();
    void initPopUpMenuForClip();
    void deleteTrackFromGui() const;
    void duplicateTrackFromGui() const;

    enum popUpMenuOptions
    {
        noOptionChosen = 0,
        deleteTrack,
        deleteAudioClip,
        duplicateTrack
    };

    std::vector<std::unique_ptr<Waveform>> waveforms{};
    juce::ValueTree& tree;
    juce::Identifier deleteTrackGui{"deleteTrackGui"};
    juce::Identifier duplicateTrackGui{"duplicateTrackGui"};
    juce::PopupMenu trackMenu;
    juce::PopupMenu clipMenu;

    uint16_t currentBoxWidth;
    uint16_t currentBoxHeight{TrackPlayerConstants::startBoxHeight};
    int currentNumOfSeconds{};
};