#pragma once

#include <juce_audio_utils/juce_audio_utils.h>
#include "../Constants.h"

class TransportController final : public juce::AudioPlayHead,
                                  public juce::Timer,
                                  public juce::ValueTree::Listener
{
public:
    explicit TransportController(juce::ValueTree& valueTree, double sampleRate);
    ~TransportController() override;

    void advancePlayHead(int numSamples);
    void setPlayingForExport(bool shouldPlay);
    int64_t& getCurrentPositionSamplesRef();
    void setProjectLength(double lengthSeconds);

    juce::Optional<PositionInfo> getPosition() const override;

private:
    void timerCallback() override;
    void valueTreePropertyChanged(juce::ValueTree&, const juce::Identifier& property) override;

    void play();
    void pause();
    void stop();
    void setPlayHeadPosition(int64_t positionSamples);

    juce::ValueTree& tree;
    double sampleRate;
    juce::CriticalSection lock;
    double projectLengthSeconds{TrackPlayerConstants::startNumOfBoxes};
    bool isPlaying{false};
    int64_t currentPositionSamples{0};

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TransportController)
};
