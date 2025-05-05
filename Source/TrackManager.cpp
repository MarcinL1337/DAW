#include "TrackManager.h"
#include <algorithm>

TrackManager::TrackManager(TrackGuiManager& trackPlayerRef, MainAudio& mainAudioRef) :
    trackGuiManager{trackPlayerRef}, mainAudio{mainAudioRef}, tree{trackPlayerRef.tree}
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
    trackGuiManager.addTrack();
    tracks.push_back(std::make_unique<AudioTrack>(mainAudio));
    return static_cast<int>(tracks.size() - 1);
}

void TrackManager::removeTrack(const int trackIndex)
{
    assert(trackIndex >= 0 && trackIndex < static_cast<int>(tracks.size()));
    tracks.erase(tracks.begin() + trackIndex);
    trackGuiManager.removeTrack(trackIndex);
}

int TrackManager::duplicateTrack(const int trackIndex)
{
    assert(trackIndex >= 0 && trackIndex < static_cast<int>(tracks.size()));
    const nlohmann::json trackJson = tracks[trackIndex]->toJson();
    return createTrackFromJson(trackJson);
}

int TrackManager::createTrackFromJson(const nlohmann::json& trackJson)
{
    const int newTrackIndex = addTrack();

    for(const auto& clipJson: trackJson["audioClips"])
    {
        if(juce::File audioFile(clipJson["path"].get<std::string>()); audioFile.existsAsFile())
        {
            const NodeID clipId = addAudioClipToTrack(newTrackIndex, audioFile);
            setOffsetOfAudioClipInSeconds(clipId, clipJson["offsetSeconds"]);
        }
    }

    setTrackProperty(newTrackIndex, AudioClipProperty::GAIN, trackJson["properties"]["gain"].get<float>());
    setTrackProperty(newTrackIndex, AudioClipProperty::PAN, trackJson["properties"]["pan"].get<float>());

    const bool isMuted = trackJson["properties"]["mute"].get<bool>();
    const bool isSoloed = trackJson["properties"]["solo"].get<bool>();
    trackGuiManager.setTrackButtonStates(newTrackIndex, isMuted, isSoloed);
    setTrackProperty(newTrackIndex, AudioClipProperty::MUTE, isMuted);
    setTrackProperty(newTrackIndex, AudioClipProperty::SOLO, isSoloed);

    return newTrackIndex;
}

NodeID TrackManager::addAudioClipToTrack(const int trackIndex, const juce::File& file) const
{
    assert(trackIndex >= 0 && trackIndex < static_cast<int>(tracks.size()));
    const NodeID newAudioClipID = tracks[trackIndex]->addAudioClip(file);
    trackGuiManager.addWaveformToTrackGui(file.getFullPathName(), trackIndex, newAudioClipID);
    return newAudioClipID;
}

// TODO: I think waveform should send signal to change offset
void TrackManager::setOffsetOfAudioClipInSeconds(const NodeID nodeID, const double offsetSeconds) const
{
    for(int i = 0; i < static_cast<int>(tracks.size()); ++i)
        if(const auto& clips = tracks[i]->getAudioClips(); std::ranges::find(clips, nodeID) != clips.end())
        {
            tracks[i]->setOffsetOfAudioClipInSeconds(nodeID, offsetSeconds);
            trackGuiManager.setOffsetOfWaveformInSeconds(i, nodeID, offsetSeconds);
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
            removeTrack(0);

        return true;
    }
    if(key.getModifiers().isShiftDown() && key.getTextCharacter() == '{')
    {
        if(!tracks.empty())
            duplicateTrack(0);

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