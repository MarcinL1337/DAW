#pragma once

#include "TrackPlayer/TrackPlayer.h"
#include "Audio/MainAudio.h"
#include <vector>
#include <utility>

class TrackManager
{
public:
    TrackManager(TrackPlayer& trackPlayerRef, MainAudio& mainAudioRef);
    ~TrackManager() = default;

    void addTrack();
    bool removeTrack(int trackId);
    bool changeTrackOrder(int trackId, int newPosition);

    NodeID addAudioClipToTrack(int trackId, const juce::File& file);
    bool removeAudioClipFromTrack(int trackId, NodeID clipId);
    bool moveAudioClipBetweenTracks(int sourceTrackId, int destTrackId, NodeID clipId);

    int getTrackIndexById(int trackId) const;

    bool keyPressed(const juce::KeyPress& key);

private:
    TrackPlayer& trackPlayer;
    MainAudio& mainAudio;

    std::vector<std::pair<int, std::vector<NodeID>>> tracks; // (trackId, vector<NodeID>)

    int nextTrackId = 0;
};
