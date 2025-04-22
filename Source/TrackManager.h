#pragma once

#include <utility>
#include <vector>

#include "Audio/AudioTrack.h"
#include "Audio/MainAudio.h"
#include "TrackPlayer/TrackPlayer.h"

class TrackManager : public juce::Component,
                     public juce::KeyListener
{
public:
    TrackManager(TrackPlayer& trackPlayerRef, MainAudio& mainAudioRef);
    ~TrackManager() override = default;

    int addTrack();
    bool removeTrack(int trackId);
    bool changeTrackOrder(int trackId, uint32_t newPosition);

    NodeID addAudioClipToTrack(int trackId, const juce::File& file) const;
    void setOffsetOfAudioClipInSeconds(NodeID nodeID, double offsetSeconds) const;
    bool removeAudioClipFromTrack(int trackId, NodeID clipId);
    bool moveAudioClipBetweenTracks(int sourceTrackId, int destTrackId, NodeID clipId);

    uint32_t getTrackPositionInVectorById(int trackId) const;

    bool keyPressed(const juce::KeyPress& key, Component* originatingComponent) override;

    void setPropertyForAllClipsInTrack(int trackId, AudioClipProperty property, bool boolValue);
    void setPropertyForAllClipsInTrack(int trackId, AudioClipProperty property, float floatValue);

private:
    TrackPlayer& trackPlayer;
    MainAudio& mainAudio;
    std::vector<std::unique_ptr<AudioTrack>> tracks;

    // TODO: possible overflow :c
    int nextTrackId{0};
};
