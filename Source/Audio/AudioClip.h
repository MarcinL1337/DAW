#pragma once
#include <juce_audio_utils/juce_audio_utils.h>
#include <juce_dsp/juce_dsp.h>
#include "../Fade/Fade.h"

class MainAudio;

class AudioClip final : public juce::AudioProcessor
{
public:
    explicit AudioClip(MainAudio& mainAudioRef);
    ~AudioClip() override = default;

    bool loadFile(const juce::File& file);
    juce::File& getFile() { return audioFile; }

    void setGain(const float gainDb) { gainProcessor.setGainDecibels(gainDb); }
    float getGain() const { return gainProcessor.getGainDecibels(); }
    void setPan(const float pan) { panProcessor.setPan(juce::jlimit(-1.0f, 1.0f, pan)); }
    void setReverb(const bool isReverbOn) { reverbProcessor.setEnabled(isReverbOn); }
    void setRoomSize(const float newRoomSizeValue);
    void setDamp(const float newDampValue);
    void setWetLevel(const float newWetLevelValue);
    void setDryLevel(const float newDryLevelValue);
    void setWidth(const float newWidthValue);
    void setFreeze(const float newFreezeValue);
    void setOffset(const int64_t newOffsetSamples) { offsetSamples = newOffsetSamples; }
    int64_t getOffset() const { return offsetSamples; }
    void setMute(const bool shouldMute) { mute = shouldMute; }
    void setSolo(const bool shouldSolo) { solo = shouldSolo; }
    bool isMuted() const { return mute; }
    bool isSoloed() const { return solo; }
    void setFadeData(const Fade::Data& fadeIn, const Fade::Data& fadeOut);
    std::pair<Fade::Data, Fade::Data> getFadeData() const { return {fadeIn, fadeOut}; }
    double getClipLengthSeconds() const
    {
        return reader ? static_cast<double>(reader->lengthInSamples) / reader->sampleRate : 0.0;
    }

    // AudioProcessor
    void prepareToPlay(double sampleRate, int samplesPerBlock) override;
    bool processBlockChecker() const;
    void releaseResources() override;
    void processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midi) override;

    const juce::String getName() const override { return "AudioClip"; }
    bool acceptsMidi() const override { return false; }
    bool producesMidi() const override { return false; }
    double getTailLengthSeconds() const override { return 0; }
    juce::AudioProcessorEditor* createEditor() override { return nullptr; }
    bool hasEditor() const override { return false; }
    int getNumPrograms() override { return 1; }
    int getCurrentProgram() override { return 0; }
    void setCurrentProgram(int) override {}
    const juce::String getProgramName(int) override { return {}; }
    void changeProgramName(int, const juce::String&) override {}
    void getStateInformation(juce::MemoryBlock&) override {}
    void setStateInformation(const void*, int) override {}

private:
    MainAudio& mainAudio;
    std::unique_ptr<juce::AudioFormatReader> reader{nullptr};
    std::unique_ptr<juce::ResamplingAudioSource> resampler{nullptr};
    std::unique_ptr<juce::AudioFormatReaderSource> readerSource{nullptr};
    juce::AudioFormatManager formatManager;
    double deviceSampleRate{};
    double fileSampleRate{};
    bool isPrepared{false};

    juce::dsp::Gain<float> gainProcessor;
    juce::dsp::Panner<float> panProcessor;
    juce::dsp::Reverb reverbProcessor;
    int64_t offsetSamples{0};
    bool mute{false};
    bool solo{false};

    juce::File audioFile;

    Fade::Data fadeIn, fadeOut;

    void applyFadeToBuffer(juce::AudioBuffer<float>& buffer, int64_t localPositionSamples) const;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(AudioClip)
};