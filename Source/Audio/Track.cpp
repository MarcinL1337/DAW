#include "Track.h"
#include "MainAudio.h"

Track::Track(MainAudio& mainAudioRef) :
    AudioProcessor(BusesProperties()
                       .withInput("Input", juce::AudioChannelSet::stereo(), true)
                       .withOutput("Output", juce::AudioChannelSet::stereo(), true)),
    mainAudio(mainAudioRef),
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
    deviceSampleRate = sampleRate;
    if(reader)
    {
        fileSampleRate = reader->sampleRate;
        readerSource = std::make_unique<juce::AudioFormatReaderSource>(reader.get(), false);
        resampler = std::make_unique<juce::ResamplingAudioSource>(readerSource.get(), false, reader->numChannels);

        resampler->setResamplingRatio(fileSampleRate / sampleRate);
        resampler->prepareToPlay(samplesPerBlock, sampleRate);
        const juce::dsp::ProcessSpec spec{sampleRate, static_cast<uint32_t>(samplesPerBlock), reader->numChannels};
        gainProcessor.prepare(spec);
        panProcessor.prepare(spec);
    }
    isPrepared = resampler && reader;
}

bool Track::processBlockChecker() const
{
    const bool muteAndSoloCheck = !mute && (!mainAudio.isAnySoloed() || solo);
    // TODO: what if someone deletes audio file?
    return isPrepared && mainAudio.isPlaying() && muteAndSoloCheck;
}

void Track::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer&)
{
    buffer.clear();
    if(!processBlockChecker())
        return;

    if(const auto* ph = getPlayHead())
    {
        auto positionInfo = ph->getPosition();
        if(!positionInfo.hasValue())
            return;

        auto optGlobalPositionSamples = positionInfo->getTimeInSamples();
        if(!optGlobalPositionSamples.hasValue())
            return;

        const auto localPositionSamples = static_cast<int64_t>(
            (static_cast<double>(*optGlobalPositionSamples - offsetSamples) / deviceSampleRate) * fileSampleRate);

        if(localPositionSamples >= 0 && localPositionSamples < reader->lengthInSamples)
        {
            const int64_t positionDifference = std::abs(readerSource->getNextReadPosition() - localPositionSamples);
            if(const auto threshold = static_cast<int64_t>(0.1 * fileSampleRate); positionDifference > threshold)
                readerSource->setNextReadPosition(localPositionSamples);

            resampler->getNextAudioBlock(juce::AudioSourceChannelInfo(&buffer, 0, buffer.getNumSamples()));
            juce::dsp::AudioBlock<float> block(buffer);
            gainProcessor.process(juce::dsp::ProcessContextReplacing(block));
            panProcessor.process(juce::dsp::ProcessContextReplacing(block));
        }
    }
}