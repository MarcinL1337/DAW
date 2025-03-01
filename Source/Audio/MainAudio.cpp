#include "MainAudio.h"

MainAudio::MainAudio()
{
    audioProcessorGraphInit();
    processorPlayer.setProcessor(&graph);
    audioDeviceManager.initialiseWithDefaultDevices(0, 2);
    audioDeviceManager.addAudioCallback(&processorPlayer);
    startTimerHz(60);
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
void MainAudio::setOffsetOfTrack(const int trackIndex, const float offset) const
{
    trackNodes[trackIndex].track->setOffset(offset);
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
    if(!transportIsPlaying)
    {
        startTime = juce::Time::getCurrentTime() - juce::RelativeTime::seconds(globalPositionSeconds);
        transportIsPlaying = true;
    }
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
    globalPositionSeconds = 0.0;
}

void MainAudio::seek(const double positionSeconds)
{
    juce::ScopedLock sl(lock);
    globalPositionSeconds = positionSeconds;
    startTime = juce::Time::getCurrentTime() - juce::RelativeTime::seconds(globalPositionSeconds);
}

double MainAudio::getGlobalPosition() const
{
    return transportIsPlaying ? (juce::Time::getCurrentTime() - startTime).inSeconds() : globalPositionSeconds;
}

void MainAudio::timerCallback()
{
    if(transportIsPlaying)
    {
        juce::ScopedLock sl(lock);
        globalPositionSeconds = (juce::Time::getCurrentTime() - startTime).inSeconds();
    }
}

void MainAudio::rebuildGraph()
{
    auto connections = graph.getConnections();
    for(const auto& connection: connections) graph.removeConnection(connection);
    for(auto& [nodeID, track]: trackNodes)
    {
        graph.addConnection({{nodeID, 0}, {outputNodeID, 0}});
        graph.addConnection({{nodeID, 1}, {outputNodeID, 1}});
    }
}
