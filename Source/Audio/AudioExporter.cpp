#include "AudioExporter.h"
#include "AudioClip.h"

AudioExporter::AudioExporter(juce::AudioProcessorGraph& graphRef, int64_t& currentPositionSamplesRef) :
    graph{graphRef}, currentPositionSamples{currentPositionSamplesRef}
{}

void AudioExporter::exportToWav(const juce::File& outputFile) const
{
    if(const auto [startSeconds, endSeconds] = calculateExportRange(); startSeconds < endSeconds)
        renderAudioToFile(outputFile, startSeconds, endSeconds, graph.getSampleRate());
}

void AudioExporter::renderAudioToFile(const juce::File& outputFile, const double startSeconds, const double endSeconds,
                                      const double sampleRate) const
{
    juce::AudioBuffer<float> mixBuffer{NUM_CHANNELS, BUFFER_SIZE};
    juce::AudioBuffer<float> clipBuffer{NUM_CHANNELS, BUFFER_SIZE};
    juce::MidiBuffer midiBuffer;
    std::unique_ptr<juce::AudioFormatWriter> writer;
    assert(createWriter(outputFile, writer));

    currentPositionSamples = static_cast<int64_t>(startSeconds * sampleRate);
    const auto endPositionSamples = static_cast<int64_t>(endSeconds * sampleRate);

    for(int samplesToWrite; currentPositionSamples < endPositionSamples; currentPositionSamples += samplesToWrite)
    {
        samplesToWrite = juce::jmin(BUFFER_SIZE, static_cast<int>(endPositionSamples - currentPositionSamples));
        mixBuffer.clear();

        for(const auto* node: graph.getNodes())
        {
            auto* audioClip = dynamic_cast<AudioClip*>(node->getProcessor());
            if(!audioClip)
                continue;

            clipBuffer.clear();
            audioClip->processBlock(clipBuffer, midiBuffer);

            for(int channel = 0; channel < NUM_CHANNELS; ++channel)
                mixBuffer.addFrom(channel, 0, clipBuffer, channel, 0, samplesToWrite);
        }
        writer->writeFromAudioSampleBuffer(mixBuffer, 0, samplesToWrite);
    }

    juce::AlertWindow::showMessageBoxAsync(juce::MessageBoxIconType::InfoIcon,
                                           "Export finished",
                                           "Successfully exported " + outputFile.getFullPathName(),
                                           "OK");
}

std::pair<double, double> AudioExporter::calculateExportRange() const
{
    double earliestStart = std::numeric_limits<double>::max();
    double latestEnd = std::numeric_limits<double>::lowest();

    for(const auto* node: graph.getNodes())
    {
        const auto* audioClip = dynamic_cast<AudioClip*>(node->getProcessor());
        if(!audioClip)
            continue;

        const double clipStart = static_cast<double>(audioClip->getOffset()) / graph.getSampleRate();
        const double clipEnd = clipStart + audioClip->getClipLengthSeconds();

        earliestStart = std::min(earliestStart, clipStart);
        latestEnd = std::max(latestEnd, clipEnd);
    }

    if(earliestStart == std::numeric_limits<double>::max())
        return {0.0, 0.0};

    return {earliestStart - 1.0, latestEnd + 1.0};
}

bool AudioExporter::createWriter(const juce::File& outputFile, std::unique_ptr<juce::AudioFormatWriter>& writer) const
{
    juce::WavAudioFormat wavFormat;

    if(!outputFile.deleteFile())
    {
        juce::AlertWindow::showMessageBoxAsync(
            juce::MessageBoxIconType::WarningIcon, "Error", "Failed to delete " + outputFile.getFullPathName(), "OK");
        return false;
    }

    std::unique_ptr<juce::OutputStream> outputStream = std::make_unique<juce::FileOutputStream>(outputFile);
    assert(outputStream);

    juce::AudioFormatWriterOptions options;
    options = options.withSampleRate(graph.getSampleRate()).withNumChannels(NUM_CHANNELS).withBitsPerSample(BIT_DEPTH);

    writer = wavFormat.createWriterFor(outputStream, options);
    assert(writer);

    return true;
}