#include "MainAudio.h"

MainAudio::MainAudio(juce::ValueTree& valueTree) : tree{valueTree}
{
    audioProcessorGraphInit();
    processorPlayer.setProcessor(&graph);
    audioDeviceManager.initialiseWithDefaultDevices(0, 2);
    audioDeviceManager.addAudioCallback(this);
    graph.setPlayHead(this);
    tree.addListener(this);
    startTimer(20);
}

void MainAudio::audioProcessorGraphInit()
{
    outputNodeID = graph
                       .addNode(std::make_unique<juce::AudioProcessorGraph::AudioGraphIOProcessor>(
                           juce::AudioProcessorGraph::AudioGraphIOProcessor::audioOutputNode))
                       ->nodeID;
}

MainAudio::~MainAudio()
{
    stopTimer();
    tree.removeListener(this);
    audioDeviceManager.removeAudioCallback(this);
    graph.clear();
}

NodeID MainAudio::addAudioClip(const juce::File& file)
{
    juce::ScopedLock sl(lock);
    auto audioClip = std::make_unique<AudioClip>(*this);
    if(!audioClip->loadFile(file))
        throw std::runtime_error("Failed to load file: " + file.getFullPathName().toStdString());

    audioClip->prepareToPlay(audioDeviceManager.getCurrentAudioDevice()->getCurrentSampleRate(),
                             audioDeviceManager.getCurrentAudioDevice()->getCurrentBufferSizeSamples());

    const auto node = graph.addNode(std::move(audioClip));
    graph.addConnection({{node->nodeID, 0}, {outputNodeID, 0}});
    graph.addConnection({{node->nodeID, 1}, {outputNodeID, 1}});
    return node->nodeID;
}

void MainAudio::removeAudioClip(const NodeID nodeID)
{
    juce::ScopedLock sl(lock);
    graph.removeNode(nodeID);
}

void MainAudio::setPanOfAudioClip(const NodeID nodeID, const float pan) const
{
    dynamic_cast<AudioClip*>(graph.getNodeForId(nodeID)->getProcessor())->setPan(pan);
}
void MainAudio::setGainOfAudioClip(const NodeID nodeID, const float gain) const
{
    dynamic_cast<AudioClip*>(graph.getNodeForId(nodeID)->getProcessor())->setGain(gain);
}
float MainAudio::getGainOfAudioClip(const NodeID nodeID) const
{
    return dynamic_cast<AudioClip*>(graph.getNodeForId(nodeID)->getProcessor())->getGain();
}

void MainAudio::setOffsetOfAudioClipInSeconds(const NodeID nodeID, const double offsetSeconds) const
{
    dynamic_cast<AudioClip*>(graph.getNodeForId(nodeID)->getProcessor())->setOffset(offsetSeconds * getSampleRate());
}
void MainAudio::setSoloOfAudioClip(const NodeID nodeID, const bool solo) const
{
    dynamic_cast<AudioClip*>(graph.getNodeForId(nodeID)->getProcessor())->setSolo(solo);
}
void MainAudio::setMuteOfAudioClip(const NodeID nodeID, const bool mute) const
{
    dynamic_cast<AudioClip*>(graph.getNodeForId(nodeID)->getProcessor())->setMute(mute);
}

void MainAudio::setReverbOfAudioClip(NodeID nodeID, bool isReverbOn) const
{
    dynamic_cast<AudioClip*>(graph.getNodeForId(nodeID)->getProcessor())->setReverb(isReverbOn);
}
void MainAudio::setRoomSizeOfAudioClip(const NodeID nodeID, const float newRoomSizeValue) const
{
    dynamic_cast<AudioClip*>(graph.getNodeForId(nodeID)->getProcessor())->setRoomSize(newRoomSizeValue);
}
void MainAudio::setDampOfAudioClip(const NodeID nodeID, const float newDampValue) const
{
    dynamic_cast<AudioClip*>(graph.getNodeForId(nodeID)->getProcessor())->setDamp(newDampValue);
}
void MainAudio::setWetLevelOfAudioClip(const NodeID nodeID, const float newWetLevelValue) const
{
    dynamic_cast<AudioClip*>(graph.getNodeForId(nodeID)->getProcessor())->setWetLevel(newWetLevelValue);
}
void MainAudio::setDryLevelOfAudioClip(const NodeID nodeID, const float newDryLevelValue) const
{
    dynamic_cast<AudioClip*>(graph.getNodeForId(nodeID)->getProcessor())->setDryLevel(newDryLevelValue);
}
void MainAudio::setWidthOfAudioClip(const NodeID nodeID, const float newWidthValue) const
{
    dynamic_cast<AudioClip*>(graph.getNodeForId(nodeID)->getProcessor())->setWidth(newWidthValue);
}
void MainAudio::setFreezeOfAudioClip(const NodeID nodeID, const float newFreezeValue) const
{
    dynamic_cast<AudioClip*>(graph.getNodeForId(nodeID)->getProcessor())->setFreeze(newFreezeValue);
}

void MainAudio::play()
{
    juce::ScopedLock sl(lock);
    transportIsPlaying = true;
    tree.setProperty(ValueTreeIDs::isPlaying, true, nullptr);
}

void MainAudio::pause()
{
    juce::ScopedLock sl(lock);
    transportIsPlaying = false;
    tree.setProperty(ValueTreeIDs::isPlaying, false, nullptr);
}

void MainAudio::stop()
{
    juce::ScopedLock sl(lock);
    transportIsPlaying = false;
    setPlayheadPosition(0);
    tree.setProperty(ValueTreeIDs::isPlaying, false, nullptr);
}

void MainAudio::setPlayheadPosition(const int64_t positionSamples)
{
    juce::ScopedLock sl(lock);
    if(transportIsPlaying)
        return;
    currentPositionSamples = positionSamples;
    const double positionInSeconds = static_cast<double>(currentPositionSamples) / getSampleRate();
    tree.setProperty("timeBarTime", positionInSeconds, nullptr);
}

juce::Optional<juce::AudioPlayHead::PositionInfo> MainAudio::getPosition() const
{
    juce::ScopedLock sl(lock);
    PositionInfo info;

    info.setTimeInSamples(currentPositionSamples);
    info.setIsPlaying(transportIsPlaying);
    info.setIsRecording(false);
    return info;
}

bool MainAudio::isAnySoloed() const
{
    juce::ScopedLock sl(lock);
    return std::ranges::any_of(
        graph.getNodes(),
        [&](const juce::AudioProcessorGraph::Node* node)
        { return node->nodeID == outputNodeID ? false : dynamic_cast<AudioClip*>(node->getProcessor())->isSoloed(); });
}

void MainAudio::valueTreePropertyChanged(juce::ValueTree&, const juce::Identifier& property)
{
    if(static_cast<int>(tree[property]) == ValueTreeConstants::doNothing)
        return;
    if(property == ValueTreeIDs::playPauseButtonClicked)
    {
        if(transportIsPlaying)
            pause();
        else
            play();
    }
    else if(property == ValueTreeIDs::stopButtonClicked)
        stop();
    else if(property == ValueTreeIDs::setPlayheadPosition)
    {
        const double positionSeconds = tree[ValueTreeIDs::setPlayheadPosition];
        const auto positionSamples = static_cast<int64_t>(positionSeconds * getSampleRate());
        setPlayheadPosition(positionSamples);
    }
    else if(property == ValueTreeIDs::isCurrentlyDraggingTimeBar)
    {
        static bool wasPlaying = false;
        if(isPlaying())
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
    else if(property == ValueTreeIDs::audioClipFadeChanged)
    {
        const auto fadeInfo = tree[ValueTreeIDs::audioClipFadeChanged];
        const NodeID clipID{static_cast<uint32_t>(static_cast<int>(fadeInfo[0]))};

        const Fade::Data fadeIn{fadeInfo[1], static_cast<Fade::Function>(static_cast<int>(fadeInfo[2]))};
        const Fade::Data fadeOut{fadeInfo[3], static_cast<Fade::Function>(static_cast<int>(fadeInfo[4]))};

        setFadeOfAudioClip(clipID, fadeIn, fadeOut);
    }
    else if(property == ValueTreeIDs::numOfSecondsChanged)
    {
        if(const int newNumOfSeconds = tree[ValueTreeIDs::numOfSecondsChanged]; newNumOfSeconds > projectLengthSeconds)
        {
            projectLengthSeconds = newNumOfSeconds;
        }
    }
}

void MainAudio::timerCallback()
{
    if(transportIsPlaying)
    {
        const double positionInSeconds = static_cast<double>(currentPositionSamples) / getSampleRate();
        tree.setProperty(ValueTreeIDs::timeBarTime, positionInSeconds, nullptr);

        if(positionInSeconds >= projectLengthSeconds)
        {
            tree.setProperty(ValueTreeIDs::stopButtonClicked, true, nullptr);
            tree.setProperty(ValueTreeIDs::stopButtonClicked, ValueTreeConstants::doNothing, nullptr);
        }
    }
}

juce::File MainAudio::getAudioClipPath(const NodeID nodeID) const
{
    return dynamic_cast<AudioClip*>(graph.getNodeForId(nodeID)->getProcessor())->getFile();
}

double MainAudio::getAudioClipOffsetInSeconds(const NodeID nodeID) const
{
    const auto offsetSamples = dynamic_cast<AudioClip*>(graph.getNodeForId(nodeID)->getProcessor())->getOffset();
    return static_cast<double>(offsetSamples) / getSampleRate();
}

void MainAudio::setFadeOfAudioClip(const NodeID nodeID, const Fade::Data& fadeIn, const Fade::Data& fadeOut) const
{
    dynamic_cast<AudioClip*>(graph.getNodeForId(nodeID)->getProcessor())->setFadeData(fadeIn, fadeOut);
}

void MainAudio::audioDeviceIOCallbackWithContext(const float* const* inputChannelData, int numInputChannels,
                                                 float* const* outputChannelData, int numOutputChannels, int numSamples,
                                                 const juce::AudioIODeviceCallbackContext& context)
{
    processorPlayer.audioDeviceIOCallbackWithContext(
        inputChannelData, numInputChannels, outputChannelData, numOutputChannels, numSamples, context);

    if(transportIsPlaying)
    {
        juce::ScopedLock sl(lock);
        currentPositionSamples += numSamples;
    }
}

void MainAudio::audioDeviceAboutToStart(juce::AudioIODevice* device)
{
    processorPlayer.audioDeviceAboutToStart(device);
}

void MainAudio::audioDeviceStopped() { processorPlayer.audioDeviceStopped(); }
std::pair<Fade::Data, Fade::Data> MainAudio::getAudioClipFadeData(const NodeID nodeID) const
{
    const auto* audioClip = dynamic_cast<AudioClip*>(graph.getNodeForId(nodeID)->getProcessor());
    assert(audioClip);
    return audioClip->getFadeData();
}
