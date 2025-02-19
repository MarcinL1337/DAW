#pragma once
#include <juce_audio_utils/juce_audio_utils.h>
#include <juce_dsp/juce_dsp.h>

class MainAudio;

#define log(x) std::cout << x << std::endl
class Track final : public juce::AudioProcessor
{
public:
    Track(MainAudio& mainAudioRef);
    ~Track() override;

    bool loadFile(const juce::File& file);

    void setNodeID(const juce::AudioProcessorGraph::NodeID newID) { nodeID = newID; }
    juce::AudioProcessorGraph::NodeID getNodeID() const { return nodeID; }

    void setGain(const float gainDb) { gainProcessor.setGainDecibels(gainDb); }
    void setPan(const float pan) { panProcessor.setPan(juce::jlimit(-1.0f, 1.0f, pan)); }
    void setOffset(const double newOffsetSeconds) { this->offsetSeconds = newOffsetSeconds; }

    // AudioProcessor
    void prepareToPlay(double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;
    void processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midi) override;

    const juce::String getName() const override { return "Track"; }
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
    juce::AudioFormatManager formatManager;
    MainAudio& mainAudio;
    std::unique_ptr<juce::AudioFormatReader> reader;
    juce::dsp::Gain<float> gainProcessor;
    juce::dsp::Panner<float> panProcessor;
    juce::AudioProcessorGraph::NodeID nodeID;
    double offsetSeconds = 0.0;
    double sampleRate = 44100.0;
    int blockSize = 512;
    bool isPrepared = false;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Track)
};