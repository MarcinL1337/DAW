#include "Track.h"
#include "MainAudio.h"

Track::Track(MainAudio& mainAudioRef) :
    AudioProcessor(BusesProperties()
                       .withInput("Input", juce::AudioChannelSet::stereo(), true)
                       .withOutput("Output", juce::AudioChannelSet::stereo(), true)),
    mainAudio(mainAudioRef),
    nodeID(juce::AudioProcessorGraph::NodeID())
{
    formatManager.registerBasicFormats();
    panProcessor.setPan(-0.5f);
    gainProcessor.setGainDecibels(-15.0f);
}

Track::~Track() {}

void Track::releaseResources()
{
    gainProcessor.reset();
    panProcessor.reset();
}

bool Track::loadFile(const juce::File& file)
{
    reader.reset(formatManager.createReaderFor(file));
    return reader != nullptr;
}

void Track::prepareToPlay(const double sampleRate, const int samplesPerBlock)
{
    this->sampleRate = sampleRate;
    this->blockSize = samplesPerBlock;
    gainProcessor.prepare({sampleRate, static_cast<juce::uint32>(samplesPerBlock), 2});
    panProcessor.prepare({sampleRate, static_cast<juce::uint32>(samplesPerBlock), 2});
    isPrepared = true;
}

void Track::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer&)
{
    buffer.clear();
    if(!isPrepared || !reader || !mainAudio.isPlaying())
        return;

    const double globalPosition = mainAudio.getGlobalPosition();
    const double localPosition = globalPosition - offsetSeconds;
    const auto startSample = static_cast<juce::int64>(localPosition * sampleRate);

    if(startSample < 0 || startSample + buffer.getNumSamples() > reader->lengthInSamples)
        return;

    reader->read(&buffer, 0, buffer.getNumSamples(), startSample, true, true);
    juce::dsp::AudioBlock<float> block(buffer);
    gainProcessor.process(juce::dsp::ProcessContextReplacing<float>(block));
    panProcessor.process(juce::dsp::ProcessContextReplacing<float>(block));
}
