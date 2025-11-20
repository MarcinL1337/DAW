#include "TrackManager.h"

TrackManager::TrackManager(TrackGuiManager& trackGuiManagerRef, MainAudio& mainAudioRef, SideMenu& sideMenuRef) :
    trackGuiManager{trackGuiManagerRef},
    audioTrackManager{mainAudioRef},
    sideMenu{sideMenuRef},
    tree{trackGuiManagerRef.tree}
{
    tree.addListener(this);
}

int TrackManager::addTrack()
{
    trackGuiManager.addTrack();
    const auto trackIndex = audioTrackManager.addAudioTrack();
    sideMenu.addTrack();
    return static_cast<int>(trackIndex);
}

void TrackManager::removeTrack(const int trackIndex)
{
    juce::Timer::callAfterDelay(1, [this, trackIndex]() { trackGuiManager.removeTrack(trackIndex); });
    sideMenu.removeTrack(trackIndex);
    audioTrackManager.removeAudioTrack(trackIndex);
}

int TrackManager::duplicateTrack(const int trackIndex)
{
    nlohmann::json trackJson = audioTrackManager.getAudioTrackJson(trackIndex);
    const auto audioFolder = getProjectAudioFolder();
    for(auto& clip: trackJson["audioClips"])
    {
        juce::File audioFile = audioFolder.getChildFile(clip["path"].get<std::string>());
        clip["path"] = audioFile.getFullPathName().toStdString();
    }

    return createTrackFromJson(trackJson);
}

int TrackManager::createTrackFromJson(const nlohmann::json& trackJson)
{
    const int newTrackIndex = addTrack();
    for(const auto& clipJson: trackJson["audioClips"])
    {
        if(juce::File audioFile{clipJson["path"].get<std::string>()}; audioFile.existsAsFile())
        {
            const NodeID clipId = addAudioClipToTrack(newTrackIndex, audioFile);
            setOffsetOfAudioClipInSeconds(clipId, newTrackIndex, clipJson["offsetSeconds"]);

            const auto fadeInData = Fade::fadeDataFromJson(clipJson["fadeIn"]);
            const auto fadeOutData = Fade::fadeDataFromJson(clipJson["fadeOut"]);

            juce::Array<juce::var> fadeInfo{static_cast<int>(clipId.uid),
                                            fadeInData.lengthSeconds,
                                            static_cast<int>(fadeInData.function),
                                            fadeOutData.lengthSeconds,
                                            static_cast<int>(fadeOutData.function)};

            tree.setProperty(ValueTreeIDs::audioClipFadeChanged, fadeInfo, nullptr);
            tree.setProperty(ValueTreeIDs::audioClipFadeChanged, ValueTreeConstants::doNothing, nullptr);
        }
    }

    audioTrackManager.setTrackProperty(
        newTrackIndex, AudioClipProperty::GAIN, trackJson["properties"]["gain"].get<float>());
    audioTrackManager.setTrackProperty(
        newTrackIndex, AudioClipProperty::PAN, trackJson["properties"]["pan"].get<float>());

    const bool isReverbOn = trackJson["properties"].value("reverb", false);
    audioTrackManager.setTrackProperty(newTrackIndex, AudioClipProperty::REVERB, isReverbOn);

    if(trackJson["properties"].contains("reverbProperties"))
    {
        const auto& reverbProps = trackJson["properties"]["reverbProperties"];
        audioTrackManager.setTrackProperty(
            newTrackIndex, ReverbClipProperty::ROOM_SIZE, reverbProps.value("roomSize", 50.0f));
        audioTrackManager.setTrackProperty(newTrackIndex, ReverbClipProperty::DAMP, reverbProps.value("damp", 50.0f));
        audioTrackManager.setTrackProperty(
            newTrackIndex, ReverbClipProperty::WET_LEVEL, reverbProps.value("wetLevel", 33.0f));
        audioTrackManager.setTrackProperty(
            newTrackIndex, ReverbClipProperty::DRY_LEVEL, reverbProps.value("dryLevel", 40.0f));
        audioTrackManager.setTrackProperty(
            newTrackIndex, ReverbClipProperty::WIDTH, reverbProps.value("width", 100.0f));
        audioTrackManager.setTrackProperty(
            newTrackIndex, ReverbClipProperty::FREEZE, reverbProps.value("freeze", 0.0f));
    }

    sideMenu.setTrackProperties(newTrackIndex, trackJson["properties"]["gain"].get<float>());

    const bool isMuted = trackJson["properties"]["mute"].get<bool>();
    const bool isSoloed = trackJson["properties"]["solo"].get<bool>();
    trackGuiManager.setTrackButtonStates(newTrackIndex, isMuted, isSoloed);
    audioTrackManager.setTrackProperty(newTrackIndex, AudioClipProperty::MUTE, isMuted);
    audioTrackManager.setTrackProperty(newTrackIndex, AudioClipProperty::SOLO, isSoloed);

    const juce::String name = trackJson["properties"]["name"].get<std::string>();
    audioTrackManager.setTrackName(newTrackIndex, name);
    trackGuiManager.setTrackName(newTrackIndex, name);

    return newTrackIndex;
}

NodeID TrackManager::addAudioClipToTrack(const int trackIndex, const juce::File& file) const
{
    const NodeID newAudioClipID = audioTrackManager.addClipToTrack(trackIndex, file);
    trackGuiManager.addWaveformToTrackGui(file.getFullPathName(), trackIndex, newAudioClipID);
    return newAudioClipID;
}

void TrackManager::setOffsetOfAudioClipInSeconds(const NodeID nodeID, const int trackIndex,
                                                 const double offsetSeconds) const
{
    audioTrackManager.setClipOffset(nodeID, offsetSeconds);
    trackGuiManager.setOffsetOfWaveformInSeconds(trackIndex, nodeID, offsetSeconds);
}

void TrackManager::valueTreePropertyChanged(juce::ValueTree&, const juce::Identifier& property)
{
    if(static_cast<int>(tree[property]) == ValueTreeConstants::doNothing)
        return;
    if(property == ValueTreeIDs::soloButtonClicked)
    {
        const int trackIndex = tree[ValueTreeIDs::soloButtonClicked];
        const bool soloValue = audioTrackManager.getTrackProperties(trackIndex).solo;
        audioTrackManager.setTrackProperty(trackIndex, AudioClipProperty::SOLO, !soloValue);
    }
    else if(property == ValueTreeIDs::muteButtonClicked)
    {
        const int trackIndex = tree[ValueTreeIDs::muteButtonClicked];
        const bool muteValue = audioTrackManager.getTrackProperties(trackIndex).mute;
        audioTrackManager.setTrackProperty(trackIndex, AudioClipProperty::MUTE, !muteValue);
    }
    else if(property == ValueTreeIDs::trackGainChanged)
    {
        const int trackIndex = tree[ValueTreeIDs::trackGainChanged][0];
        const float gainValue = tree[ValueTreeIDs::trackGainChanged][1];
        audioTrackManager.setTrackProperty(trackIndex, AudioClipProperty::GAIN, gainValue);
    }
    else if(property == ValueTreeIDs::trackPanChanged)
    {
        const int trackIndex = tree[ValueTreeIDs::trackPanChanged][0];
        const float panValue = tree[ValueTreeIDs::trackPanChanged][1];
        audioTrackManager.setTrackProperty(trackIndex, AudioClipProperty::PAN, panValue);
    }
    else if(property == ValueTreeIDs::trackReverbChanged)
    {
        const int trackIndex = tree[ValueTreeIDs::trackReverbChanged][0];
        const bool isReverbOn = tree[ValueTreeIDs::trackReverbChanged][1];
        audioTrackManager.setTrackProperty(trackIndex, AudioClipProperty::REVERB, isReverbOn);
    }
    else if(property == ValueTreeIDs::trackRoomSizeChanged)
    {
        const int trackIndex = tree[ValueTreeIDs::trackRoomSizeChanged][0];
        const float roomSizeValue = tree[ValueTreeIDs::trackRoomSizeChanged][1];
        audioTrackManager.setTrackProperty(trackIndex, ReverbClipProperty::ROOM_SIZE, roomSizeValue);
    }
    else if(property == ValueTreeIDs::trackDampChanged)
    {
        const int trackIndex = tree[ValueTreeIDs::trackDampChanged][0];
        const float dampValue = tree[ValueTreeIDs::trackDampChanged][1];
        audioTrackManager.setTrackProperty(trackIndex, ReverbClipProperty::DAMP, dampValue);
    }
    else if(property == ValueTreeIDs::trackWetLevelChanged)
    {
        const int trackIndex = tree[ValueTreeIDs::trackWetLevelChanged][0];
        const float wetValue = tree[ValueTreeIDs::trackWetLevelChanged][1];
        audioTrackManager.setTrackProperty(trackIndex, ReverbClipProperty::WET_LEVEL, wetValue);
    }
    else if(property == ValueTreeIDs::trackDryLevelChanged)
    {
        const int trackIndex = tree[ValueTreeIDs::trackDryLevelChanged][0];
        const float dryValue = tree[ValueTreeIDs::trackDryLevelChanged][1];
        audioTrackManager.setTrackProperty(trackIndex, ReverbClipProperty::DRY_LEVEL, dryValue);
    }
    else if(property == ValueTreeIDs::trackWidthChanged)
    {
        const int trackIndex = tree[ValueTreeIDs::trackWidthChanged][0];
        const float widthValue = tree[ValueTreeIDs::trackWidthChanged][1];
        audioTrackManager.setTrackProperty(trackIndex, ReverbClipProperty::WIDTH, widthValue);
    }
    else if(property == ValueTreeIDs::trackFreezeChanged)
    {
        const int trackIndex = tree[ValueTreeIDs::trackFreezeChanged][0];
        const float freezeValue = tree[ValueTreeIDs::trackFreezeChanged][1];
        audioTrackManager.setTrackProperty(trackIndex, ReverbClipProperty::FREEZE, freezeValue);
    }
    else if(property == ValueTreeIDs::trackNameChanged)
    {
        const int trackIndex = tree[ValueTreeIDs::trackNameChanged][0];
        const juce::String nameValue = tree[ValueTreeIDs::trackNameChanged][1];
        audioTrackManager.setTrackName(trackIndex, nameValue);
    }
    else if(property == ValueTreeIDs::deleteTrackGui)
    {
        const int trackIndex = tree[ValueTreeIDs::deleteTrackGui];
        removeTrack(trackIndex);
    }
    else if(property == ValueTreeIDs::duplicateTrackGui)
    {
        const int trackIndex = tree[ValueTreeIDs::duplicateTrackGui];
        duplicateTrack(trackIndex);
    }
    else if(property == ValueTreeIDs::deleteAudioClip)
    {
        const int trackIndex = tree[ValueTreeIDs::deleteAudioClip][0];
        const int audioClipUid = tree[ValueTreeIDs::deleteAudioClip][1];
        const NodeID audioClipID{static_cast<uint32_t>(audioClipUid)};
        const bool result = audioTrackManager.removeClipFromTrack(trackIndex, audioClipID);
        assert(result);
    }
    else if(property == ValueTreeIDs::copyAudioClip)
    {
        const int audioClipUid = tree[ValueTreeIDs::copyAudioClip];
        const NodeID audioClipID{static_cast<uint32_t>(audioClipUid)};
        currentlyCopiedClipFilePath.emplace(audioTrackManager.getClipPath(audioClipID));
    }
    else if(property == ValueTreeIDs::pasteAudioClip)
    {
        if(currentlyCopiedClipFilePath.has_value())
        {
            const int trackIndex = tree[ValueTreeIDs::pasteAudioClip][0];
            const float pastedClipOffset = tree[ValueTreeIDs::pasteAudioClip][1];

            const auto newNodeId = addAudioClipToTrack(trackIndex, currentlyCopiedClipFilePath.value());
            setOffsetOfAudioClipInSeconds(newNodeId, trackIndex, pastedClipOffset);
            currentlyCopiedClipFilePath = std::nullopt;
        }
    }
    else if(property == ValueTreeIDs::splitAudioClip)
    {
        const int trackIndex = tree[ValueTreeIDs::splitAudioClip][0];
        const int audioClipUid = tree[ValueTreeIDs::splitAudioClip][1];
        const NodeID audioClipID{static_cast<uint32_t>(audioClipUid)};
        const float waveformSplit = tree[ValueTreeIDs::splitAudioClip][2];

        auto [firstFile, firstOffset, secondFile, secondOffset] =
            audioTrackManager.splitClip(trackIndex, audioClipID, waveformSplit, getProjectAudioFolder());

        const auto firstSplitNodeId = addAudioClipToTrack(trackIndex, firstFile);
        setOffsetOfAudioClipInSeconds(firstSplitNodeId, trackIndex, firstOffset);

        const auto secondSplitNodeId = addAudioClipToTrack(trackIndex, secondFile);
        setOffsetOfAudioClipInSeconds(secondSplitNodeId, trackIndex, secondOffset);

        const bool result = audioTrackManager.removeClipFromTrack(trackIndex, audioClipID);
        assert(result);
    }
    else if(property == ValueTreeIDs::reorderTracks)
    {
        const int fromIndex = tree[ValueTreeIDs::reorderTracks][0];
        const int toIndex = tree[ValueTreeIDs::reorderTracks][1];
        audioTrackManager.moveTrack(fromIndex, toIndex);
    }
    else if(property == ValueTreeIDs::clearAllTracks)
    {
        clearAllTracks();
    }
    else if(property == ValueTreeIDs::createTrackFromJson)
    {
        const juce::String trackJsonString = tree[ValueTreeIDs::createTrackFromJson];
        auto trackJson = nlohmann::json::parse(trackJsonString.toStdString());
        createTrackFromJson(trackJson);
    }
    else if(property == ValueTreeIDs::exportTracksToJson)
    {
        const auto projectJson = audioTrackManager.exportToJson();
        tree.setProperty(ValueTreeIDs::tracksJsonExported, projectJson.dump(4).data(), nullptr);
        tree.setProperty(ValueTreeIDs::exportTracksToJson, ValueTreeConstants::doNothing, nullptr);
    }
    else if(property == ValueTreeIDs::addAudioFileToNewTrack)
    {
        const juce::String audioFilePath = tree[ValueTreeIDs::addAudioFileToNewTrack];
        if(const juce::File audioFile{audioFilePath}; audioFile.existsAsFile())
        {
            const int newTrackIndex = addTrack();
            std::ignore = addAudioClipToTrack(newTrackIndex, audioFile);
        }
    }
    else if(property == ValueTreeIDs::moveAudioClip)
    {
        const int audioClipUid = tree[ValueTreeIDs::moveAudioClip][0];
        const int targetTrackIndex = tree[ValueTreeIDs::moveAudioClip][1];
        const float newOffsetSeconds = tree[ValueTreeIDs::moveAudioClip][2];
        const NodeID audioClipID{static_cast<uint32_t>(audioClipUid)};

        const auto& clipPath = audioTrackManager.getClipPath(audioClipID);
        const auto& [fadeIn, fadeOut] = audioTrackManager.getClipFadeData(audioClipID);

        const auto newClipID = addAudioClipToTrack(targetTrackIndex, clipPath);
        setOffsetOfAudioClipInSeconds(newClipID, targetTrackIndex, newOffsetSeconds);

        const juce::Array<juce::var> fadeInfo{static_cast<int>(newClipID.uid),
                                              fadeIn.lengthSeconds,
                                              static_cast<int>(fadeIn.function),
                                              fadeOut.lengthSeconds,
                                              static_cast<int>(fadeOut.function)};
        tree.setProperty(ValueTreeIDs::audioClipFadeChanged, fadeInfo, nullptr);
        tree.setProperty(ValueTreeIDs::audioClipFadeChanged, ValueTreeConstants::doNothing, nullptr);
    }
}

void TrackManager::clearAllTracks()
{
    trackGuiManager.clearAllTracks();
    sideMenu.clearAllTracks();
    audioTrackManager.clearAllTracks();
}

juce::File TrackManager::getProjectAudioFolder() const
{
    if(!tree.hasProperty(ValueTreeIDs::projectAudioDir))
        return juce::File{};

    return juce::File{tree[ValueTreeIDs::projectAudioDir].toString()};
}