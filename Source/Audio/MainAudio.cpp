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

void MainAudio::addTrack(const juce::File& file)
{
    juce::ScopedLock sl(lock);
    const auto track = new Track(*this);
    if(!track->loadFile(file))
    {
        throw std::runtime_error("Failed to load file: " + file.getFullPathName().toStdString());
    }
    const auto node = graph.addNode(std::unique_ptr<Track>(track));
    const TrackNode newTrackNode = {node->nodeID, track};
    trackNodes.add(newTrackNode);

    track->setNodeID(node->nodeID);
    track->prepareToPlay(audioDeviceManager.getCurrentAudioDevice()->getCurrentSampleRate(),
                         audioDeviceManager.getCurrentAudioDevice()->getCurrentBufferSizeSamples());
    rebuildGraph();
}

void MainAudio::removeTrack(const Track* track)
{
    juce::ScopedLock sl(lock);
    graph.removeNode(track->getNodeID());
    trackNodes.removeIf([&](const TrackNode& tn) { return tn.track == track; });
    rebuildGraph();
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
