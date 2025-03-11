#pragma once

#include "Track.h"

class MainAudio final : public juce::AudioPlayHead
{
public:
    struct TrackNode
    {
        juce::AudioProcessorGraph::NodeID nodeID;
        // TODO: maybe change to std::unique_ptr
        Track* track{};
        bool operator==(const TrackNode& other) const { return nodeID == other.nodeID && track == other.track; }
    };

    MainAudio();
    ~MainAudio() override;

    int addTrack(const juce::File& file);  // returns index of the track in trackNodes
    void removeTrack(const Track* track);
    void setPanOfTrack(int trackIndex, float pan) const;
    void setGainOfTrack(int trackIndex, float gain) const;
    void setOffsetOfTrackInSeconds(int trackIndex, double offsetSeconds) const;
    void setSoloOfTrack(int trackIndex, bool solo) const;
    void setMuteOfTrack(int trackIndex, bool mute) const;

    void play();
    void pause();
    void stop();
    void seek(int64_t positionSamples);
    bool isPlaying() const { return transportIsPlaying; }
    juce::Optional<PositionInfo> getPosition() const override;
    bool isAnySoloed() const;
    double getSampleRate() const { return audioDeviceManager.getCurrentAudioDevice()->getCurrentSampleRate(); }

private:
    void audioProcessorGraphInit();
    void rebuildGraph();

    juce::AudioDeviceManager audioDeviceManager;
    juce::AudioProcessorPlayer processorPlayer;
    juce::AudioProcessorGraph graph;
    juce::AudioProcessorGraph::NodeID outputNodeID;
    juce::Array<TrackNode> trackNodes;

    juce::Time startTime;
    bool transportIsPlaying{false};
    mutable int64_t currentPositionSamples{0};
    juce::CriticalSection lock;
};