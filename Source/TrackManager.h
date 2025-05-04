#pragma once

#include <utility>
#include <vector>

#include "Audio/AudioTrack.h"
#include "Audio/MainAudio.h"
#include "Constants.h"
#include "TrackPlayer/TrackGuiManager.h"

class TrackManager final : public juce::Component,
                           public juce::KeyListener,
                           public juce::ValueTree::Listener
{
public:
    TrackManager(TrackGuiManager& trackPlayerRef, MainAudio& mainAudioRef);
    ~TrackManager() override = default;

    int addTrack();
    void removeTrack(int trackIndex);
    int duplicateTrack(int trackIndex);
    int createTrackFromJson(const nlohmann::json& trackJson);
    bool changeTrackOrder(int trackIndex, int newPosition);

    NodeID addAudioClipToTrack(int trackIndex, const juce::File& file) const;
    void setOffsetOfAudioClipInSeconds(NodeID nodeID, double offsetSeconds) const;
    bool removeAudioClipFromTrack(int trackIndex, NodeID clipId);
    bool moveAudioClipBetweenTracks(int sourceTrackIndex, int destTrackIndex, NodeID clipId);

    bool keyPressed(const juce::KeyPress& key, Component* originatingComponent) override;

    void setTrackProperty(int trackIndex, AudioClipProperty property, bool boolValue) const;
    void setTrackProperty(int trackIndex, AudioClipProperty property, float floatValue) const;
    TrackProperties getTrackProperties(int trackIndex) const;

    void valueTreePropertyChanged(juce::ValueTree& treeWhosePropertyHasChanged, const juce::Identifier& property) override;

private:
    TrackGuiManager& trackPlayer;
    MainAudio& mainAudio;
    juce::ValueTree& tree;
    std::vector<std::unique_ptr<AudioTrack>> tracks;
};
