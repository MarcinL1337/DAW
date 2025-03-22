#pragma once

#include "Track.h"
using NodeID = juce::AudioProcessorGraph::NodeID;

class MainAudio final : public juce::AudioPlayHead
{
public:
    MainAudio();
    ~MainAudio() override;
    NodeID addTrack(const juce::File& file);  // returns index of the track in trackNodes
    void removeTrack(NodeID nodeID);
    void setPanOfTrack(NodeID nodeID, float pan) const;
    void setGainOfTrack(NodeID nodeID, float gain) const;
    void setOffsetOfTrackInSeconds(NodeID nodeID, double offsetSeconds) const;
    void setSoloOfTrack(NodeID nodeID, bool solo) const;
    void setMuteOfTrack(NodeID nodeID, bool mute) const;

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
    NodeID outputNodeID;

    juce::Time startTime;
    bool transportIsPlaying{false};
    mutable int64_t currentPositionSamples{0};
    juce::CriticalSection lock;
};