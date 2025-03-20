#pragma once

#include "Track.h"

class MainAudio final : public juce::AudioPlayHead
{
public:
    MainAudio();
    ~MainAudio() override;

    juce::AudioProcessorGraph::NodeID addTrack(const juce::File& file);  // returns index of the track in trackNodes
    void removeTrack(juce::AudioProcessorGraph::NodeID nodeID);
    void setPanOfTrack(juce::AudioProcessorGraph::NodeID nodeID, float pan) const;
    void setGainOfTrack(juce::AudioProcessorGraph::NodeID nodeID, float gain) const;
    void setOffsetOfTrackInSeconds(juce::AudioProcessorGraph::NodeID nodeID, double offsetSeconds) const;
    void setSoloOfTrack(juce::AudioProcessorGraph::NodeID nodeID, bool solo) const;
    void setMuteOfTrack(juce::AudioProcessorGraph::NodeID nodeID, bool mute) const;

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

    juce::Time startTime;
    bool transportIsPlaying{false};
    mutable int64_t currentPositionSamples{0};
    juce::CriticalSection lock;
};