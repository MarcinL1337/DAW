#pragma once

#include "Audio/AudioTrackManager.h"
#include "SideMenu/SideMenu.h"
#include "TrackPlayer/TrackGuiManager.h"

class TrackManager final : public juce::Component,
                           public juce::ValueTree::Listener
{
public:
    TrackManager(TrackGuiManager& trackGuiManagerRef, MainAudio& mainAudioRef, SideMenu& sideMenuRef);
    ~TrackManager() override = default;

    int addTrack();
    void removeTrack(int trackIndex);
    int duplicateTrack(int trackIndex);
    int createTrackFromJson(const nlohmann::json& trackJson);
    void clearAllTracks();

    NodeID addAudioClipToTrack(int trackIndex, const juce::File& file) const;
    void setOffsetOfAudioClipInSeconds(NodeID nodeID, int trackIndex, double offsetSeconds) const;
    void valueTreePropertyChanged(juce::ValueTree& treeWhosePropertyHasChanged,
                                  const juce::Identifier& property) override;

private:
    juce::File getProjectAudioFolder() const;

    TrackGuiManager& trackGuiManager;
    AudioTrackManager audioTrackManager;
    SideMenu& sideMenu;
    juce::ValueTree& tree;
    std::optional<juce::File> currentlyCopiedClipFilePath{std::nullopt};
};
