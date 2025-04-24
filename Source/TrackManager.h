#pragma once

#include <utility>
#include <vector>

#include "Audio/AudioTrack.h"
#include "Audio/MainAudio.h"
#include "TrackPlayer/TrackPlayer.h"

class TrackManager final : public juce::Component,
                           public juce::KeyListener,
                           public juce::ValueTree::Listener
{
public:
    TrackManager(TrackPlayer& trackPlayerRef, MainAudio& mainAudioRef);
    ~TrackManager() override = default;

    int addTrack();
    void removeTrack(int trackIndex);
    bool changeTrackOrder(int trackIndex, int newPosition);

    NodeID addAudioClipToTrack(int trackIndex, const juce::File& file) const;
    void setOffsetOfAudioClipInSeconds(NodeID nodeID, double offsetSeconds) const;
    bool removeAudioClipFromTrack(int trackIndex, NodeID clipId);
    bool moveAudioClipBetweenTracks(int sourceTrackIndex, int destTrackIndex, NodeID clipId);

    bool keyPressed(const juce::KeyPress& key, Component* originatingComponent) override;

    void setPropertyForAllClipsInTrack(int trackIndex, AudioClipProperty property, bool boolValue);
    void setPropertyForAllClipsInTrack(int trackIndex, AudioClipProperty property, float floatValue);

    void valueTreePropertyChanged(juce::ValueTree& treeWhosePropertyHasChanged,
                                  const juce::Identifier& property) override;

private:
    TrackPlayer& trackPlayer;
    MainAudio& mainAudio;
    juce::ValueTree& tree;
    std::vector<std::unique_ptr<AudioTrack>> tracks;
};
