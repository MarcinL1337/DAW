#include "Track.h"
#include "MainAudio.h"

Track::Track(MainAudio& mainAudioRef) :
    AudioProcessor(BusesProperties()
                       .withInput("Input", juce::AudioChannelSet::stereo(), true)
                       .withOutput("Output", juce::AudioChannelSet::stereo(), true)),
    mainAudio(mainAudioRef),
    nodeID(juce::AudioProcessorGraph::NodeID()),
    resampler(nullptr),
    readerSource(nullptr)
{
    formatManager.registerBasicFormats();
}

void Track::releaseResources()
{
    gainProcessor.reset();
    panProcessor.reset();
}

bool Track::loadFile(const juce::File& file)
{
    std::unique_ptr<juce::AudioFormatReader> temp_reader(formatManager.createReaderFor(file));
    reader = std::move(temp_reader);
    return reader != nullptr;
}

void Track::prepareToPlay(const double sampleRate, const int samplesPerBlock)
{
    if(reader)
    {
        fileSampleRate = reader->sampleRate;
        readerSource = std::make_unique<juce::AudioFormatReaderSource>(reader.get(), false);
        resampler = std::make_unique<juce::ResamplingAudioSource>(readerSource.get(), false, reader->numChannels);

        resampler->setResamplingRatio(fileSampleRate / sampleRate);
        resampler->prepareToPlay(samplesPerBlock, sampleRate);
    }
    gainProcessor.prepare({sampleRate, static_cast<uint32_t>(samplesPerBlock), reader->numChannels});
    panProcessor.prepare({sampleRate, static_cast<uint32_t>(samplesPerBlock), reader->numChannels});
    isPrepared = resampler && reader;
}

void Track::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer&)
{
    buffer.clear();
    if(!isPrepared || !mainAudio.isPlaying())
        return;

    const double globalPosition = mainAudio.getGlobalPosition();
    const double localPosition = globalPosition - offsetSeconds;
    const double startSample = localPosition * fileSampleRate;
    if(startSample < 0 || startSample >= reader->lengthInSamples - buffer.getNumSamples() ||
       abs(readerSource->getNextReadPosition() - startSample) > 1500)
    {
        // TODO: check if tracks are in sync (because I don't think they are)
        readerSource->setNextReadPosition(startSample);
        return;
    }
    resampler->getNextAudioBlock(juce::AudioSourceChannelInfo(&buffer, 0, buffer.getNumSamples()));

    juce::dsp::AudioBlock<float> block(buffer);
    gainProcessor.process(juce::dsp::ProcessContextReplacing(block));
    panProcessor.process(juce::dsp::ProcessContextReplacing(block));
}
