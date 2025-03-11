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

int MainAudio::addTrack(const juce::File& file)
{
    juce::ScopedLock sl(lock);
    const auto track = new Track(*this);
    if(!track->loadFile(file))
        throw std::runtime_error("Failed to load file: " + file.getFullPathName().toStdString());

    const auto node = graph.addNode(std::unique_ptr<Track>(track));
    const TrackNode newTrackNode = {node->nodeID, track};
    trackNodes.add(newTrackNode);

    track->setNodeID(node->nodeID);
    track->prepareToPlay(audioDeviceManager.getCurrentAudioDevice()->getCurrentSampleRate(),
                         audioDeviceManager.getCurrentAudioDevice()->getCurrentBufferSizeSamples());
    rebuildGraph();
    return trackNodes.indexOf(newTrackNode);
}

void MainAudio::removeTrack(const Track* track)
{
    juce::ScopedLock sl(lock);
    graph.removeNode(track->getNodeID());
    trackNodes.removeIf([&](const TrackNode& tn) { return tn.track == track; });
    rebuildGraph();
}

void MainAudio::setPanOfTrack(const int trackIndex, const float pan) const
{
    trackNodes[trackIndex].track->setPan(pan);
}
void MainAudio::setGainOfTrack(const int trackIndex, const float gain) const
{
    trackNodes[trackIndex].track->setGain(gain);
}
void MainAudio::setOffsetOfTrackInSeconds(const int trackIndex, const double offsetSeconds) const
{
    trackNodes[trackIndex].track->setOffset(offsetSeconds * getSampleRate());
}
void MainAudio::setSoloOfTrack(const int trackIndex, const bool solo) const
{
    trackNodes[trackIndex].track->setSolo(solo);
}
void MainAudio::setMuteOfTrack(const int trackIndex, const bool mute) const
{
    trackNodes[trackIndex].track->setMute(mute);
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
    for(auto& [nodeID, track]: trackNodes)
    {
        graph.addConnection({{nodeID, 0}, {outputNodeID, 0}});
        graph.addConnection({{nodeID, 1}, {outputNodeID, 1}});
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
    return std::ranges::any_of(trackNodes, [&](const TrackNode& tn) { return tn.track->isSoloed(); });
}