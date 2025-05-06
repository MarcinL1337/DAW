#pragma once

#include <juce_gui_extra/juce_gui_extra.h>
#include <cassert>
#include "../Audio/MainAudio.h"
#include "../Constants.h"
#include "Timeline.h"
#include "TrackGui.h"
#include "TrackGuiComponent.h"
#include "TrackPlayerSideMenu.h"

class TrackGuiManager final : public juce::Component,
                              public juce::ValueTree::Listener
{
public:
    explicit TrackGuiManager(juce::ValueTree& parentTree);
    TrackGuiManager(const TrackGuiManager&) = delete;
    TrackGuiManager& operator=(const TrackGuiManager&) = delete;
    ~TrackGuiManager() override = default;

    uint16_t getCurrentNumberOfTracks() const { return trackGuiVector.size(); }

private:
    friend class TrackManager;
    friend class TrackGui;

    void paint(juce::Graphics& g) override;
    void resized() override;

    void makeNewTrackGui();
    void viewportsInit();
    void addTrack();
    void setTrackButtonStates(int trackIndex, bool muted, bool soloed) const;
    void removeTrack(int trackIndex);
    void addWaveformToTrackGui(const juce::String& newAudioFilePath, int trackIndex, NodeID newAudioClipID) const;
    void setOffsetOfWaveformInSeconds(int trackIndex, NodeID audioClipID, double offsetSeconds) const;

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