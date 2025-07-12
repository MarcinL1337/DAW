#pragma once

#include <nlohmann/json.hpp>
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
    juce::String name{"Track"};
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
    void setProperty(const juce::String& stringValue);

    TrackProperties getProperties() const;

    std::vector<NodeID>& getAudioClips() { return audioClips; }
    const std::vector<NodeID>& getAudioClips() const { return audioClips; }

    nlohmann::json toJson() const;

private:
    std::vector<NodeID> audioClips;
    TrackProperties properties;
    MainAudio& mainAudio;
};