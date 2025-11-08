#include "AudioExporter.h"
#include "AudioClip.h"

AudioExporter::AudioExporter(juce::AudioPlayHead& playHeadRef, juce::AudioProcessorGraph& graphRef,
                             const juce::AudioProcessorGraph::NodeID& outputNodeIDRef,
                             int64_t& currentPositionSamplesRef, bool& transportIsPlayingRef) :
    playHead{playHeadRef},
    graph{graphRef},
    outputNodeID{outputNodeIDRef},
    currentPositionSamples{currentPositionSamplesRef},
    transportIsPlaying{transportIsPlayingRef}
{}

void AudioExporter::exportToWav(const juce::File& outputFile)
{
    const auto [startSeconds, endSeconds] = calculateExportRange();

    if(startSeconds >= endSeconds)
        return;

    renderAudioToFile(outputFile, startSeconds, endSeconds);
}

void AudioExporter::renderAudioToFile(const juce::File& outputFile, const double startSeconds, const double endSeconds)
{
    const auto sampleRate = graph.getSampleRate();
    const auto totalSamples = static_cast<int64_t>((endSeconds - startSeconds) * sampleRate);

    juce::AudioBuffer<float> mixBuffer(NUM_CHANNELS, BUFFER_SIZE);
    juce::AudioBuffer<float> clipBuffer(NUM_CHANNELS, BUFFER_SIZE);

    std::unique_ptr<juce::AudioFormatWriter> writer;
    if(!createWriter(outputFile, writer))
        return;

    int64_t samplesWritten = 0;
    transportIsPlaying = true;

    while(samplesWritten < totalSamples)
    {
        const int samplesToWrite = juce::jmin(BUFFER_SIZE, static_cast<int>(totalSamples - samplesWritten));
        mixBuffer.clear();

        currentPositionSamples = static_cast<int64_t>(startSeconds * sampleRate) + samplesWritten;

        for(const auto* node: graph.getNodes())
        {
            if(node->nodeID == outputNodeID)
                continue;

            auto* audioClip = dynamic_cast<AudioClip*>(node->getProcessor());
            if(!audioClip)
                continue;

            clipBuffer.clear();
            audioClip->setPlayHead(&playHead);

            juce::MidiBuffer midiBuffer;
            audioClip->processBlock(clipBuffer, midiBuffer);

            for(int channel = 0; channel < NUM_CHANNELS; ++channel)
                mixBuffer.addFrom(channel, 0, clipBuffer, channel, 0, samplesToWrite);
        }

        writer->writeFromAudioSampleBuffer(mixBuffer, 0, samplesToWrite);
        samplesWritten += samplesToWrite;
    }

    transportIsPlaying = false;

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
        if(node->nodeID == outputNodeID)
            continue;

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

bool AudioExporter::createWriter(const juce::File& outputFile, std::unique_ptr<juce::AudioFormatWriter>& writer)
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