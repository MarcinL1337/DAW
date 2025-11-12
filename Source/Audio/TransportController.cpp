#include "TransportController.h"

TransportController::TransportController(juce::ValueTree& valueTree, const double sampleRate) :
    tree{valueTree}, sampleRate{sampleRate}
{
    tree.addListener(this);
    startTimer(20);
}

TransportController::~TransportController()
{
    stopTimer();
    tree.removeListener(this);
}

void TransportController::advancePlayHead(const int numSamples)
{
    juce::ScopedLock sl{lock};
    if(isPlaying)
        currentPositionSamples += numSamples;
}

void TransportController::setPlayingForExport(const bool shouldPlay)
{
    juce::ScopedLock sl{lock};
    isPlaying = shouldPlay;
}

int64_t& TransportController::getCurrentPositionSamplesRef() { return currentPositionSamples; }

juce::Optional<juce::AudioPlayHead::PositionInfo> TransportController::getPosition() const
{
    juce::ScopedLock sl{lock};
    PositionInfo info{};
    info.setTimeInSamples(currentPositionSamples);
    info.setIsPlaying(isPlaying);
    info.setIsRecording(false);
    return info;
}

void TransportController::timerCallback()
{
    if(!isPlaying)
        return;

    const double positionInSeconds = static_cast<double>(currentPositionSamples) / sampleRate;
    tree.setProperty(ValueTreeIDs::timeBarTime, positionInSeconds, nullptr);

    if(positionInSeconds >= projectLengthSeconds)
    {
        tree.setProperty(ValueTreeIDs::stopButtonClicked, true, nullptr);
        tree.setProperty(ValueTreeIDs::stopButtonClicked, ValueTreeConstants::doNothing, nullptr);
    }
}

void TransportController::valueTreePropertyChanged(juce::ValueTree&, const juce::Identifier& property)
{
    if(static_cast<int>(tree[property]) == ValueTreeConstants::doNothing)
        return;

    if(property == ValueTreeIDs::playPauseButtonClicked)
    {
        if(isPlaying)
            pause();
        else
            play();
    }
    else if(property == ValueTreeIDs::stopButtonClicked)
    {
        stop();
    }
    else if(property == ValueTreeIDs::setPlayheadPosition)
    {
        const double positionSeconds = tree[ValueTreeIDs::setPlayheadPosition];
        const auto positionSamples = static_cast<int64_t>(positionSeconds * sampleRate);
        setPlayHeadPosition(positionSamples);
    }
    else if(property == ValueTreeIDs::isCurrentlyDraggingTimeBar)
    {
        static bool wasPlaying{false};
        if(isPlaying)
        {
            wasPlaying = true;
            pause();
        }
        else if(wasPlaying)
        {
            wasPlaying = false;
            play();
        }
    }
    else if(property == ValueTreeIDs::numOfSecondsChanged)
    {
        if(const int newNumOfSeconds = tree[ValueTreeIDs::numOfSecondsChanged]; newNumOfSeconds > projectLengthSeconds)
        {
            projectLengthSeconds = newNumOfSeconds;
        }
    }
}

void TransportController::play()
{
    juce::ScopedLock sl{lock};
    isPlaying = true;
    tree.setProperty(ValueTreeIDs::isPlaying, true, nullptr);
}

void TransportController::pause()
{
    juce::ScopedLock sl{lock};
    isPlaying = false;
    tree.setProperty(ValueTreeIDs::isPlaying, false, nullptr);
}

void TransportController::stop()
{
    juce::ScopedLock sl{lock};
    isPlaying = false;
    setPlayHeadPosition(0);
    tree.setProperty(ValueTreeIDs::isPlaying, false, nullptr);
}

void TransportController::setPlayHeadPosition(const int64_t positionSamples)
{
    juce::ScopedLock sl{lock};
    if(isPlaying)
        return;

    currentPositionSamples = positionSamples;
    const double positionInSeconds = static_cast<double>(currentPositionSamples) / sampleRate;
    tree.setProperty(ValueTreeIDs::timeBarTime, positionInSeconds, nullptr);
}
