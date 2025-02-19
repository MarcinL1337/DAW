#pragma once

#include "Track.h"

class MainAudio final : juce::Timer
{
public:
    struct TrackNode
    {
        juce::AudioProcessorGraph::NodeID nodeID;
        Track* track{};
    };

    MainAudio();
    ~MainAudio() override;

    void addTrack(const juce::File& file);
    void removeTrack(const Track* track);
    void play();
    void stop();
    void seek(double positionSeconds);
    bool isPlaying() const { return transportIsPlaying; }
    double getGlobalPosition() const;

private:
    juce::AudioProcessorGraph graph;
    void audioProcessorGraphInit();

    juce::AudioProcessorPlayer processorPlayer;
    juce::Array<TrackNode> trackNodes;
    juce::AudioDeviceManager audioDeviceManager;

    juce::AudioProcessorGraph::NodeID outputNodeID;

    juce::Time startTime;
    double globalPositionSeconds = 0.0;
    bool transportIsPlaying = false;
    juce::CriticalSection lock;

    void timerCallback() override;
    void rebuildGraph();
};
