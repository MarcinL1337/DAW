#pragma once

#include "Track.h"

class MainAudio final : juce::Timer
{
public:
    struct TrackNode
    {
        juce::AudioProcessorGraph::NodeID nodeID;
        Track* track{};
        bool operator==(const TrackNode& other) const { return nodeID == other.nodeID && track == other.track; }
    };

    MainAudio();
    ~MainAudio() override;

    int addTrack(const juce::File& file);  // returns index of the track in trackNodes
    void removeTrack(const Track* track);
    void setPanOfTrack(int trackIndex, float pan) const;
    void setGainOfTrack(int trackIndex, float gain) const;
    void setOffsetOfTrack(int trackIndex, float offset) const;
    void setSoloOfTrack(int trackIndex, bool solo) const;
    void setMuteOfTrack(int trackIndex, bool mute) const;

    void play();
    void pause();
    void stop();
    void seek(double positionSeconds);
    bool isPlaying() const { return transportIsPlaying; }
    double getGlobalPosition() const;

private:
    juce::AudioDeviceManager audioDeviceManager;
    juce::AudioProcessorPlayer processorPlayer;
    juce::AudioProcessorGraph graph;
    juce::AudioProcessorGraph::NodeID outputNodeID;
    void audioProcessorGraphInit();
    juce::Array<TrackNode> trackNodes;

    juce::Time startTime;
    double globalPositionSeconds = 0.0;
    bool transportIsPlaying = false;
    juce::CriticalSection lock;

    void timerCallback() override;
    void rebuildGraph();
};
