#pragma once

#include <vector>
#include "../Constants.h"
#include "MainAudio.h"

enum class AudioClipProperty
{
    MUTE,
    SOLO,
    GAIN,
    PAN
};

struct TrackProperties
{
    float gain{0.0f};
    float pan{0.0f};
    bool mute{false};
    bool solo{false};
};

class AudioTrack
{
public:
    explicit AudioTrack(MainAudio& mainAudioRef);
    ~AudioTrack();

    NodeID addAudioClip(const juce::File& file);
    bool removeAudioClip(NodeID clipId);
    void setOffsetOfAudioClipInSeconds(NodeID nodeID, double offsetSeconds) const;

    void setProperty(AudioClipProperty property, bool boolValue);
    void setProperty(AudioClipProperty property, float floatValue);

    std::vector<NodeID>& getAudioClips() { return audioClips; }
    const std::vector<NodeID>& getAudioClips() const { return audioClips; }

private:
    std::vector<NodeID> audioClips;
    TrackProperties properties;
    MainAudio& mainAudio;
};