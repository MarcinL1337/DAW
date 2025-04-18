#pragma once

#include <utility>
#include <vector>

#include "Audio/MainAudio.h"
#include "TrackPlayer/TrackPlayer.h"

enum class AudioClipProperty
{
    MUTE,
    SOLO,
    GAIN,
    PAN
};

class TrackManager
{
public:
    TrackManager(TrackPlayer& trackPlayerRef, MainAudio& mainAudioRef);
    ~TrackManager() = default;

    int addTrack();
    bool removeTrack(int trackId);
    bool changeTrackOrder(int trackId, int newPosition);

    NodeID addAudioClipToTrack(int trackId, const juce::File& file);
    void setOffsetOfAudioClipInSeconds(NodeID nodeID, double offsetSeconds) const;
    bool removeAudioClipFromTrack(int trackId, NodeID clipId);
    bool moveAudioClipBetweenTracks(int sourceTrackId, int destTrackId, NodeID clipId);

    int getTrackIndexById(int trackId) const;

    bool keyPressed(const juce::KeyPress& key);

    void setPropertyForAllClipsInTrack(int trackId, AudioClipProperty property, bool boolValue);
    void setPropertyForAllClipsInTrack(int trackId, AudioClipProperty property, float floatValue);

private:
    TrackPlayer& trackPlayer;
    MainAudio& mainAudio;

    // TODO: Add a struct with track state, because when adding a new AudioClip to a track, appropriate attributes need
    // to be assigned (e.g., mute, gain, pan)
    std::vector<std::pair<int, std::vector<NodeID>>> tracks;  // (trackId, vector<NodeID>)

    int nextTrackId{0};
};
