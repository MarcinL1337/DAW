#pragma once

#include <utility>
#include <vector>

#include "Audio/AudioTrack.h"
#include "Audio/MainAudio.h"
#include "Constants.h"
#include "SideMenu/SideMenu.h"
#include "TrackPlayer/TrackGuiManager.h"

class TrackManager final : public juce::Component,
                           public juce::KeyListener,
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
    void setOffsetOfAudioClipInSeconds(NodeID nodeID, double offsetSeconds) const;
    bool removeAudioClipFromTrack(const int trackIndex, const NodeID clipId) const;
    bool copyAudioClip(const int trackIndex, const NodeID clipId);
    bool moveAudioClipBetweenTracks(int sourceTrackIndex, int destTrackIndex, NodeID clipId);  // to be implemented

    bool keyPressed(const juce::KeyPress& key, Component* originatingComponent) override;

    void setTrackProperty(int trackIndex, AudioClipProperty property, bool boolValue) const;
    void setTrackProperty(int trackIndex, AudioClipProperty property, float floatValue) const;
    void setTrackProperty(int trackIndex, juce::String stringValue) const;
    TrackProperties getTrackProperties(int trackIndex) const;

    void valueTreePropertyChanged(juce::ValueTree& treeWhosePropertyHasChanged,
                                  const juce::Identifier& property) override;

    nlohmann::json exportTracksToJson() const;

private:
    TrackGuiManager& trackGuiManager;
    MainAudio& mainAudio;
    SideMenu& sideMenu;
    juce::ValueTree& tree;
    std::vector<std::unique_ptr<AudioTrack>> tracks;
    std::optional<juce::File> currentlyCopiedClipFilePath{std::nullopt};

    void changeTrackOrder(int fromIndex, int toIndex);
};
