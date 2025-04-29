#pragma once

#include "../Constants.h"
#include "AudioClip.h"
using NodeID = juce::AudioProcessorGraph::NodeID;

class MainAudio final : public juce::AudioPlayHead,
                        public juce::ValueTree::Listener,
                        public juce::Timer
{
public:
    explicit MainAudio(juce::ValueTree& valueTree);
    ~MainAudio() override;

    void play();
    void pause();
    void stop();
    void setPlayheadPosition(int64_t positionSamples);
    bool isPlaying() const { return transportIsPlaying; }
    juce::Optional<PositionInfo> getPosition() const override;
    bool isAnySoloed() const;
    double getSampleRate() const { return audioDeviceManager.getCurrentAudioDevice()->getCurrentSampleRate(); }

    void timerCallback() override;

private:
    void audioProcessorGraphInit();
    void rebuildGraph();

    void valueTreePropertyChanged(juce::ValueTree& treeWhosePropertyHasChanged,
                                  const juce::Identifier& property) override;

    friend class AudioTrack;

    NodeID addAudioClip(const juce::File& file);
    void removeAudioClip(NodeID nodeID);
    void setPanOfAudioClip(NodeID nodeID, float pan) const;
    void setGainOfAudioClip(NodeID nodeID, float gain) const;
    void setOffsetOfAudioClipInSeconds(NodeID nodeID, double offsetSeconds) const;
    void setSoloOfAudioClip(NodeID nodeID, bool solo) const;
    void setMuteOfAudioClip(NodeID nodeID, bool mute) const;

    juce::ValueTree& tree;

    juce::AudioDeviceManager audioDeviceManager;
    juce::AudioProcessorPlayer processorPlayer;
    juce::AudioProcessorGraph graph;
    NodeID outputNodeID;
    juce::Time startTime;
    bool transportIsPlaying{false};
    mutable int64_t currentPositionSamples{0};
    juce::CriticalSection lock;
};
