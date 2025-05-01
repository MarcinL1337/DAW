#include "TrackManager.h"
#include <algorithm>

TrackManager::TrackManager(TrackPlayer& trackPlayerRef, MainAudio& mainAudioRef) :
    trackPlayer(trackPlayerRef), mainAudio(mainAudioRef), tree(trackPlayerRef.tree)
{
    // TODO: "W chuj mi się to nie podoba"~LilMarcin
    juce::Timer::callAfterDelay(200,
                                [this]
                                {
                                    if(auto* parent = getParentComponent())
                                        parent->addKeyListener(this);
                                });
    tree.addListener(this);
}

int TrackManager::addTrack()
{
    trackPlayer.addTrack();
    tracks.push_back(std::make_unique<AudioTrack>(mainAudio));
    return static_cast<int>(tracks.size() - 1);
}

void TrackManager::removeTrack(const int trackIndex)
{
    assert(trackIndex >= 0 && trackIndex < static_cast<int>(tracks.size()));
    tracks.erase(tracks.begin() + trackIndex);
    trackPlayer.removeTrack(trackIndex);
}

NodeID TrackManager::addAudioClipToTrack(const int trackIndex, const juce::File& file) const
{
    assert(trackIndex >= 0 && trackIndex < static_cast<int>(tracks.size()));
    trackPlayer.addWaveformToTrackGui(file.getFullPathName(), trackIndex);
    return tracks[trackIndex]->addAudioClip(file);
}

void TrackManager::setOffsetOfAudioClipInSeconds(const NodeID nodeID, const double offsetSeconds) const
{
    for(const auto& track: tracks)
        if(const auto& clips = track->getAudioClips(); std::ranges::find(clips, nodeID) != clips.end())
        {
            track->setOffsetOfAudioClipInSeconds(nodeID, offsetSeconds);
            return;
        }
}

bool TrackManager::keyPressed(const juce::KeyPress& key, Component* originatingComponent)
{
    if(key.getModifiers().isShiftDown() && key.getTextCharacter() == '+')
    {
        addTrack();
        return true;
    }
    if(key.getModifiers().isShiftDown() && key.getTextCharacter() == '_')
    {
        if(!tracks.empty())
            removeTrack(static_cast<int>(tracks.size() - 1));
        return true;
    }
    return false;
}

void TrackManager::setTrackProperty(const int trackIndex, const AudioClipProperty property, const bool boolValue) const
{
    assert(trackIndex >= 0 && trackIndex < static_cast<int>(tracks.size()));
    tracks[trackIndex]->setProperty(property, boolValue);
}

void TrackManager::setTrackProperty(const int trackIndex, const AudioClipProperty property,
                                    const float floatValue) const
{
    assert(trackIndex >= 0 && trackIndex < static_cast<int>(tracks.size()));
    tracks[trackIndex]->setProperty(property, floatValue);
}

TrackProperties TrackManager::getTrackProperties(const int trackIndex) const
{
    assert(trackIndex >= 0 && trackIndex < static_cast<int>(tracks.size()));
    return tracks[trackIndex]->getProperties();
}

void TrackManager::valueTreePropertyChanged(juce::ValueTree&, const juce::Identifier& property)
{
    if(static_cast<int>(tree[property.toString()]) == ValueTreeConstants::doNothing)
        return;
    if(property.toString() == "newAudioFile")
    {
        const juce::var newAudioFilePath = tree["newAudioFile"];

        const auto index = addTrack();
        addAudioClipToTrack(index, juce::File(newAudioFilePath));
    }
    if(property.toString() == "soloButtonClicked")
    {
        const int trackIndex = tree["soloButtonClicked"];
        const bool soloValue = getTrackProperties(trackIndex).solo;
        setTrackProperty(trackIndex, AudioClipProperty::SOLO, !soloValue);
    }
    if(property.toString() == "muteButtonClicked")
    {
        const int trackIndex = tree["muteButtonClicked"];
        const bool muteValue = getTrackProperties(trackIndex).mute;
        setTrackProperty(trackIndex, AudioClipProperty::MUTE, !muteValue);
    }
}