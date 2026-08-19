#pragma once

#include <optional>
#include <utility>
#include <vector>
#include "AudioClipSplitter.h"
#include "AudioTrack.h"

class AudioTrackManager
{
public:
    explicit AudioTrackManager(MainAudio& mainAudioRef);
    ~AudioTrackManager() = default;

    size_t addAudioTrack();
    void removeAudioTrack(int trackIndex);
    nlohmann::json getAudioTrackJson(int trackIndex) const;
    void moveTrack(int fromIndex, int toIndex);

    NodeID addClipToTrack(int trackIndex, const juce::File& file) const;
    bool removeClipFromTrack(int trackIndex, NodeID clipId) const;
    void setClipOffset(NodeID clipId, double offsetSeconds) const;

    void setTrackProperty(int trackIndex, AudioClipProperty property, bool boolValue) const;
    void setTrackProperty(int trackIndex, AudioClipProperty property, float floatValue) const;
    void setTrackProperty(int trackIndex, ReverbClipProperty property, float floatValue) const;
    void setTrackName(int trackIndex, const juce::String& stringValue) const;
    TrackProperties getTrackProperties(int trackIndex) const;

    SplitClipResult splitClip(int trackIndex, NodeID clipId, float ratio, const juce::File& audioDir) const;

    juce::File getClipPath(NodeID clipId) const;
    double getClipOffset(NodeID clipId) const;
    std::pair<Fade::Data, Fade::Data> getClipFadeData(NodeID clipId) const;

    nlohmann::json exportToJson() const;
    void clearAllTracks();

private:
    MainAudio& mainAudio;
    std::vector<std::unique_ptr<AudioTrack>> tracks;
};
