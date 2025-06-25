#include "AudioClip.h"
#include "MainAudio.h"

AudioClip::AudioClip(MainAudio& mainAudioRef):
    AudioProcessor(BusesProperties()
                       .withInput("Input", juce::AudioChannelSet::stereo(), true)
                       .withOutput("Output", juce::AudioChannelSet::stereo(), true)),
    mainAudio{mainAudioRef},
    resampler{nullptr},
    readerSource{nullptr}
{
    formatManager.registerBasicFormats();
}

void AudioClip::releaseResources()
{
    gainProcessor.reset();
    panProcessor.reset();
}

bool AudioClip::loadFile(const juce::File& file)
{
    audioFile = file;
    std::unique_ptr<juce::AudioFormatReader> temp_reader(formatManager.createReaderFor(audioFile));
    reader = std::move(temp_reader);
    return reader != nullptr;
}

void AudioClip::prepareToPlay(const double sampleRate, const int samplesPerBlock)
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

bool AudioClip::processBlockChecker() const
{
    // TODO: isAnySoloed value should be in value tree, mainAudio ref should be out
    const bool muteAndSoloCheck = !mute && (!mainAudio.isAnySoloed() || solo);
    // TODO: what if someone deletes audio file?
    // TODO: isPlaying value should be in value tree, mainAudio ref should be out
    // alternative: isPlaying from audioPlayHead in processBlock?
    return isPrepared && mainAudio.isPlaying() && muteAndSoloCheck;
}

// TODO: Sometimes audio clips get out of sync, e.g., when muting and unmuting tracks. Fix audio clip synchronization.
void AudioClip::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer&)
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

            applyFadeToBuffer(buffer, localPositionSamples);
        }
    }
}

void AudioClip::applyFadeToBuffer(juce::AudioBuffer<float>& buffer, const int64_t localPositionSamples) const
{
    const auto totalLengthSeconds = static_cast<double>(reader->lengthInSamples) / fileSampleRate;

    for(int sample = 0; sample < buffer.getNumSamples(); ++sample)
    {
        const auto globalSample = localPositionSamples + sample;
        const auto timePositionSeconds = static_cast<double>(globalSample) / fileSampleRate;

        const float fadeMultiplier =
            Fade::getFadeMultiplier(timePositionSeconds, totalLengthSeconds, fadeData[0], fadeData[1]);

        for(int channel = 0; channel < buffer.getNumChannels(); ++channel)
            buffer.getWritePointer(channel)[sample] *= fadeMultiplier;
    }
}

void AudioClip::setFadeData(const Fade::Data& fadeIn, const Fade::Data& fadeOut)
{
    fadeData[0] = fadeIn;
    fadeData[1] = fadeOut;
}
