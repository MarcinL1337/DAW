#pragma once

#include <cassert>
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

    juce::File getAudioClipPath(NodeID nodeID) const;
    double getAudioClipOffsetInSeconds(NodeID nodeID) const;
    void setFadeOfAudioClip(NodeID nodeID, const Fade::Data& fadeIn, const Fade::Data& fadeOut) const;

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
    float getGainOfAudioClip(NodeID nodeID) const;
    void setOffsetOfAudioClipInSeconds(NodeID nodeID, double offsetSeconds) const;
    void setSoloOfAudioClip(NodeID nodeID, bool solo) const;
    void setMuteOfAudioClip(NodeID nodeID, bool mute) const;
    std::pair<Fade::Data, Fade::Data> getAudioClipFadeData(NodeID nodeID) const;

    void setReverbOfAudioClip(NodeID nodeID, bool mute) const;
    void setRoomSizeOfAudioClip(NodeID nodeID, float newRoomSizeValue) const;
    void setDampOfAudioClip(NodeID nodeID, float newDampValue) const;
    void setWetLevelOfAudioClip(NodeID nodeID, float newWetLevelValue) const;
    void setDryLevelOfAudioClip(NodeID nodeID, float newDryLevelValue) const;
    void setWidthOfAudioClip(NodeID nodeID, float newWidthValue) const;
    void setFreezeOfAudioClip(NodeID nodeID, float newFreezeValue) const;

    juce::ValueTree& tree;

    juce::AudioDeviceManager audioDeviceManager;
    juce::AudioProcessorPlayer processorPlayer;
    juce::AudioProcessorGraph graph;
    NodeID outputNodeID;
    juce::Time startTime;
    bool transportIsPlaying{false};
    mutable int64_t currentPositionSamples{0};
    double projectLengthSeconds{TrackPlayerConstants::startNumOfBoxes};
    juce::CriticalSection lock;
};
