#pragma once
#include <juce_audio_utils/juce_audio_utils.h>
#include "../Constants.h"

class AudioClip;

class AudioExporter
{
public:
    AudioExporter(juce::AudioPlayHead& playHeadRef, juce::AudioProcessorGraph& graphRef,
                  const juce::AudioProcessorGraph::NodeID& outputNodeIDRef, int64_t& currentPositionSamplesRef,
                  bool& transportIsPlayingRef);
    void exportToWav(const juce::File& outputFile);

private:
    void renderAudioToFile(const juce::File& outputFile, double startSeconds, double endSeconds);
    std::pair<double, double> calculateExportRange() const;
    bool createWriter(const juce::File& outputFile, std::unique_ptr<juce::AudioFormatWriter>& writer);
    void updateTransportState(int64_t position, bool isPlaying);

    juce::AudioPlayHead& playHead;
    juce::AudioProcessorGraph& graph;
    const juce::AudioProcessorGraph::NodeID& outputNodeID;
    int64_t& currentPositionSamples;
    bool& transportIsPlaying;

    static constexpr int BUFFER_SIZE = 512;
    static constexpr int NUM_CHANNELS = 2;
    static constexpr int BIT_DEPTH = 24;
};
