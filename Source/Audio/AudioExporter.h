#pragma once
#include <juce_audio_utils/juce_audio_utils.h>
#include "../Constants.h"

class AudioClip;

class AudioExporter
{
public:
    AudioExporter(juce::AudioProcessorGraph& graphRef, int64_t& currentPositionSamplesRef);
    void exportToWav(const juce::File& outputFile) const;

private:
    void renderAudioToFile(const juce::File& outputFile, double startSeconds, double endSeconds,
                           double sampleRate) const;
    std::pair<double, double> calculateExportRange() const;
    bool createWriter(const juce::File& outputFile, std::unique_ptr<juce::AudioFormatWriter>& writer) const;

    juce::AudioProcessorGraph& graph;
    int64_t& currentPositionSamples;

    static constexpr int BUFFER_SIZE = 512;
    static constexpr int NUM_CHANNELS = 2;
    static constexpr int BIT_DEPTH = 24;
};
