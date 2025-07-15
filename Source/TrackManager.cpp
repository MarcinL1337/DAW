#include "TrackManager.h"
#include <algorithm>

TrackManager::TrackManager(TrackGuiManager& trackGuiManagerRef, MainAudio& mainAudioRef, SideMenu& sideMenuRef) :
    trackGuiManager{trackGuiManagerRef}, mainAudio{mainAudioRef}, sideMenu{sideMenuRef}, tree{trackGuiManagerRef.tree}
{
    trackGuiManagerRef.addKeyListener(this);
    tree.addListener(this);
}

int TrackManager::addTrack()
{
    trackGuiManager.addTrack();
    tracks.push_back(std::make_unique<AudioTrack>(mainAudio));
    sideMenu.addTrack();
    return static_cast<int>(tracks.size() - 1);
}

void TrackManager::removeTrack(const int trackIndex)
{
    assert(trackIndex >= 0 && trackIndex < static_cast<int>(tracks.size()));
    tracks.erase(tracks.begin() + trackIndex);
    juce::Timer::callAfterDelay(1, [this, trackIndex]() { trackGuiManager.removeTrack(trackIndex); });
    sideMenu.removeTrack(trackIndex);
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

    sideMenu.setTrackProperties(newTrackIndex, trackJson["properties"]["gain"].get<float>() /* delay, reverb, ... */);

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

bool TrackManager::removeAudioClipFromTrack(const int trackIndex, const NodeID clipId) const
{
    assert(trackIndex >= 0 && trackIndex < static_cast<int>(tracks.size()));

    return tracks[trackIndex]->removeAudioClip(clipId);
}

bool TrackManager::keyPressed(const juce::KeyPress& key, Component* originatingComponent)
{
    if(key.getModifiers().isShiftDown() && key.getTextCharacter() == '+')
    {
        addTrack();
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

void TrackManager::setTrackProperty(const int trackIndex, const ReverbClipProperty property,
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
    if(static_cast<int>(tree[property]) == ValueTreeConstants::doNothing)
        return;
    if(property == ValueTreeIDs::newAudioFile)
    {
        const juce::var newAudioFilePath = tree[ValueTreeIDs::newAudioFile];

        const auto index = addTrack();
        addAudioClipToTrack(index, juce::File(newAudioFilePath));
    }
    else if(property == ValueTreeIDs::soloButtonClicked)
    {
        const int trackIndex = tree[ValueTreeIDs::soloButtonClicked];
        const bool soloValue = getTrackProperties(trackIndex).solo;
        setTrackProperty(trackIndex, AudioClipProperty::SOLO, !soloValue);
    }
    else if(property == ValueTreeIDs::muteButtonClicked)
    {
        const int trackIndex = tree[ValueTreeIDs::muteButtonClicked];
        const bool muteValue = getTrackProperties(trackIndex).mute;
        setTrackProperty(trackIndex, AudioClipProperty::MUTE, !muteValue);
    }
    else if(property == ValueTreeIDs::trackGainChanged)
    {
        const int trackIndex = tree[ValueTreeIDs::trackGainChanged][0];
        const float gainValue = tree[ValueTreeIDs::trackGainChanged][1];
        setTrackProperty(trackIndex, AudioClipProperty::GAIN, gainValue);
    }
    else if(property == ValueTreeIDs::trackPanChanged)
    {
        const int trackIndex = tree[ValueTreeIDs::trackPanChanged][0];
        const float panValue = tree[ValueTreeIDs::trackPanChanged][1];
        setTrackProperty(trackIndex, AudioClipProperty::PAN, panValue);
    }
    else if(property == ValueTreeIDs::trackReverbChanged)
    {
        const int trackIndex = tree[ValueTreeIDs::trackReverbChanged][0];
        const bool isReverbOn = tree[ValueTreeIDs::trackReverbChanged][1];
        setTrackProperty(trackIndex, AudioClipProperty::REVERB, isReverbOn);
    }
    else if(property == ValueTreeIDs::trackRoomSizeChanged)
    {
        const int trackIndex = tree[ValueTreeIDs::trackRoomSizeChanged][0];
        const float roomSizeValue = tree[ValueTreeIDs::trackRoomSizeChanged][1];
        setTrackProperty(trackIndex, ReverbClipProperty::ROOM_SIZE, roomSizeValue);
    }
    else if(property == ValueTreeIDs::trackDampChanged)
    {
        const int trackIndex = tree[ValueTreeIDs::trackDampChanged][0];
        const float dampValue = tree[ValueTreeIDs::trackDampChanged][1];
        setTrackProperty(trackIndex, ReverbClipProperty::DAMP, dampValue);
    }
    else if(property == ValueTreeIDs::trackWetLevelChanged)
    {
        const int trackIndex = tree[ValueTreeIDs::trackWetLevelChanged][0];
        const float wetValue = tree[ValueTreeIDs::trackWetLevelChanged][1];
        setTrackProperty(trackIndex, ReverbClipProperty::WET_LEVEL, wetValue);
    }
    else if(property == ValueTreeIDs::trackDryLevelChanged)
    {
        const int trackIndex = tree[ValueTreeIDs::trackDryLevelChanged][0];
        const float dryValue = tree[ValueTreeIDs::trackDryLevelChanged][1];
        setTrackProperty(trackIndex, ReverbClipProperty::DRY_LEVEL, dryValue);
    }
    else if(property == ValueTreeIDs::trackWidthChanged)
    {
        const int trackIndex = tree[ValueTreeIDs::trackWidthChanged][0];
        const float widthValue = tree[ValueTreeIDs::trackWidthChanged][1];
        setTrackProperty(trackIndex, ReverbClipProperty::WIDTH, widthValue);
    }
    else if(property == ValueTreeIDs::trackFreezeChanged)
    {
        const int trackIndex = tree[ValueTreeIDs::trackFreezeChanged][0];
        const float freezeValue = tree[ValueTreeIDs::trackFreezeChanged][1];
        setTrackProperty(trackIndex, ReverbClipProperty::FREEZE, freezeValue);
    }
    else if(property == ValueTreeIDs::deleteTrackGui)
    {
        const int trackIndex = tree[ValueTreeIDs::deleteTrackGui];
        assert(trackIndex >= 0 && trackIndex < static_cast<int>(tracks.size()));
        removeTrack(trackIndex);
    }
    else if(property == ValueTreeIDs::duplicateTrackGui)
    {
        const int trackIndex = tree[ValueTreeIDs::duplicateTrackGui];
        assert(trackIndex >= 0 && trackIndex < static_cast<int>(tracks.size()));
        duplicateTrack(trackIndex);
    }
    else if(property == ValueTreeIDs::deleteAudioClip)
    {
        const int trackIndex = tree[ValueTreeIDs::deleteAudioClip][0];
        const int audioClipUid = tree[ValueTreeIDs::deleteAudioClip][1];
        const NodeID audioClipID{static_cast<uint32_t>(audioClipUid)};

        assert(removeAudioClipFromTrack(trackIndex, audioClipID));
    }
    else if(property == ValueTreeIDs::copyAudioClip)
    {
        const int audioClipUid = tree[ValueTreeIDs::copyAudioClip];
        const NodeID audioClipID{static_cast<uint32_t>(audioClipUid)};

        currentlyCopiedClipFilePath.emplace(mainAudio.getAudioClipPath(audioClipID));
    }
    else if(property == ValueTreeIDs::pasteAudioClip)
    {
        if(currentlyCopiedClipFilePath.has_value())
        {
            const int trackIndex = tree[ValueTreeIDs::pasteAudioClip][0];
            const float pastedClipOffset = tree[ValueTreeIDs::pasteAudioClip][1];

            const auto newNodeId = addAudioClipToTrack(trackIndex, currentlyCopiedClipFilePath.value());
            setOffsetOfAudioClipInSeconds(newNodeId, pastedClipOffset);
            currentlyCopiedClipFilePath = std::nullopt;
        }
    }
    else if(property == ValueTreeIDs::reorderTracks)
    {
        const int fromIndex = tree[ValueTreeIDs::reorderTracks][0];
        const int toIndex = tree[ValueTreeIDs::reorderTracks][1];
        changeTrackOrder(fromIndex, toIndex);
    }
}

// TODO: maybe this shouldn't be in this class. Add this to TrackManager refactor
void TrackManager::changeTrackOrder(const int fromIndex, const int toIndex)
{
    auto trackToMove = std::move(tracks[fromIndex]);
    tracks.erase(tracks.begin() + fromIndex);
    tracks.insert(tracks.begin() + toIndex, std::move(trackToMove));
}