#pragma once

#include <juce_gui_extra/juce_gui_extra.h>
#include <cassert>
#include "../Audio/MainAudio.h"
#include "../Constants.h"
#include "Timeline.h"
#include "TrackGui.h"
#include "TrackGuiComponent.h"
#include "TrackPlayerSideMenu.h"

class TrackPlayer final : public juce::Component,
                          public juce::ValueTree::Listener
{
public:
    explicit TrackPlayer(juce::ValueTree& parentTree);
    TrackPlayer(const TrackPlayer&) = delete;
    TrackPlayer& operator=(const TrackPlayer&) = delete;
    ~TrackPlayer() override = default;

    uint16_t getCurrentNumberOfTracks() const { return trackGuiVector.size(); }

private:
    friend class TrackManager;
    void paint(juce::Graphics& g) override;
    void resized() override;

    void makeNewTrackGui(const juce::String& newAudioFilePath = "", const NodeID newAudioClipID = NodeID());
    void viewportsInit();
    void addTrack(const juce::String& newAudioFilePath = "");
    void removeTrack(int trackIndex);
    void addWaveformToTrackGui(const juce::String& newAudioFilePath, int trackIndex = -1,
                               NodeID newAudioClipID = NodeID(1));
    void setOffsetOfWaveformInSeconds(int trackIndex, NodeID audioClipID, double offsetSeconds);
    TrackGui* findFirstEmptyTrackGui() const;

    void valueTreePropertyChanged(juce::ValueTree& treeWhosePropertyHasChanged,
                                  const juce::Identifier& property) override;

    void changeTrackGuiBoxWidthAndPropagate(const int newBoxWidthPercentage);

    juce::Viewport trackPlayerViewport{};
    juce::Viewport timelineViewport{};
    juce::Viewport trackPlayerSideMenuViewport{};

    juce::ValueTree& tree;

    int currentNumOfSeconds{TrackPlayerConstants::startNumOfBoxes};
    Timeline timeline;
    TrackPlayerSideMenu trackPlayerSideMenu;
    TrackGuiComponent trackGuiComponent;

    std::vector<std::unique_ptr<TrackGui>> trackGuiVector{};

    const int trackButtonsSize{30};
    uint16_t baseTrackGuiBoxWidth{TrackPlayerConstants::startBoxWidth};
    uint16_t currentTrackGuiBoxWidth{baseTrackGuiBoxWidth};
    uint16_t currentTrackGuiBoxHeight{TrackPlayerConstants::startBoxHeight};
};