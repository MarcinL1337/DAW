#include "MainAudio.h"

MainAudio::MainAudio()
{
    audioProcessorGraphInit();
    processorPlayer.setProcessor(&graph);
    audioDeviceManager.initialiseWithDefaultDevices(0, 2);
    audioDeviceManager.addAudioCallback(&processorPlayer);
    graph.setPlayHead(this);
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
    audioDeviceManager.removeAudioCallback(&processorPlayer);
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
    rebuildGraph();
    return node->nodeID;
}

void MainAudio::removeAudioClip(const NodeID nodeID)
{
    juce::ScopedLock sl(lock);
    graph.removeNode(nodeID);
    rebuildGraph();
}

void MainAudio::setPanOfAudioClip(const NodeID nodeID, const float pan) const
{
    dynamic_cast<AudioClip*>(graph.getNodeForId(nodeID)->getProcessor())->setPan(pan);
}
void MainAudio::setGainOfAudioClip(const NodeID nodeID, const float gain) const
{
    dynamic_cast<AudioClip*>(graph.getNodeForId(nodeID)->getProcessor())->setGain(gain);
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

void MainAudio::play()
{
    juce::ScopedLock sl(lock);
    const auto timeOffset = juce::RelativeTime::seconds(static_cast<double>(currentPositionSamples) / getSampleRate());
    startTime = juce::Time::getCurrentTime() - timeOffset;
    transportIsPlaying = true;
}

void MainAudio::pause()
{
    juce::ScopedLock sl(lock);
    transportIsPlaying = false;
}

void MainAudio::stop()
{
    juce::ScopedLock sl(lock);
    transportIsPlaying = false;
    currentPositionSamples = 0;
}

void MainAudio::seek(const int64_t positionSamples)
{
    juce::ScopedLock sl(lock);
    currentPositionSamples = positionSamples;
}

void MainAudio::rebuildGraph()
{
    juce::ScopedLock sl(lock);
    auto connections = graph.getConnections();
    for(const auto& connection: connections) graph.removeConnection(connection);
    for(auto& node: graph.getNodes())
    {
        if(node->nodeID == outputNodeID)
            continue;
        graph.addConnection({{node->nodeID, 0}, {outputNodeID, 0}});
        graph.addConnection({{node->nodeID, 1}, {outputNodeID, 1}});
    }
}

juce::Optional<juce::AudioPlayHead::PositionInfo> MainAudio::getPosition() const
{
    juce::ScopedLock sl(lock);
    PositionInfo info;
    int64_t currentSamples;

    if(transportIsPlaying)
    {
        const double elapsedSeconds = (juce::Time::getCurrentTime() - startTime).inSeconds();
        currentSamples = static_cast<int64_t>(elapsedSeconds * getSampleRate());
        currentPositionSamples = currentSamples;
    }
    else
    {
        currentSamples = currentPositionSamples;
    }

    info.setTimeInSamples(currentSamples);
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