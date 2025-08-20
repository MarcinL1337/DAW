#include "TrackManager.h"
#include <algorithm>

TrackManager::TrackManager(TrackGuiManager& trackGuiManagerRef, MainAudio& mainAudioRef, SideMenu& sideMenuRef) :
    trackGuiManager{trackGuiManagerRef}, mainAudio{mainAudioRef}, sideMenu{sideMenuRef}, tree{trackGuiManagerRef.tree}
{
    trackGuiManagerRef.addKeyListener(this);
    tree.addListener(this);
    handleSplitClipsDirCreation();
}

TrackManager::~TrackManager() { assert(tempClipsFolder.deleteRecursively()); }

void TrackManager::handleSplitClipsDirCreation() const
{
    if(tempClipsFolder.exists())
    {
        assert(tempClipsFolder.deleteRecursively());
    }

    const auto result = tempClipsFolder.createDirectory();
    assert(result.wasOk());
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
    juce::Timer::callAfterDelay(1, [this, trackIndex]() { trackGuiManager.removeTrack(trackIndex); });
    sideMenu.removeTrack(trackIndex);
    tracks.erase(tracks.begin() + trackIndex);
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

    setTrackProperty(newTrackIndex, AudioClipProperty::GAIN, trackJson["properties"]["gain"].get<float>());
    setTrackProperty(newTrackIndex, AudioClipProperty::PAN, trackJson["properties"]["pan"].get<float>());

    sideMenu.setTrackProperties(newTrackIndex, trackJson["properties"]["gain"].get<float>() /* delay, reverb, ... */);

    const bool isMuted = trackJson["properties"]["mute"].get<bool>();
    const bool isSoloed = trackJson["properties"]["solo"].get<bool>();
    trackGuiManager.setTrackButtonStates(newTrackIndex, isMuted, isSoloed);
    setTrackProperty(newTrackIndex, AudioClipProperty::MUTE, isMuted);
    setTrackProperty(newTrackIndex, AudioClipProperty::SOLO, isSoloed);

    const juce::String name = trackJson["properties"]["name"].get<std::string>();
    trackGuiManager.setTrackName(newTrackIndex, name);

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

void TrackManager::handleWriteToFile(juce::AudioFormatReader& reader, const juce::AudioFormatManager& formatManager,
                                     const juce::File& destFile, const int numOfSamplesToWrite,
                                     const int readerOffsetInSamples)
{
    // TODO: Think about reserving memory for split clip in chunks, not all at once as below
    juce::AudioBuffer<float> buffer(static_cast<int>(reader.numChannels), numOfSamplesToWrite);
    reader.read(&buffer, 0, numOfSamplesToWrite, readerOffsetInSamples, true, true);

    auto outputStream = destFile.createOutputStream();
    assert(outputStream);
    const auto fileExtension = destFile.getFileExtension();
    auto* writer = formatManager.findFormatForFileExtension(fileExtension)
                       ->createWriterFor(outputStream.release(),
                                         reader.sampleRate,
                                         reader.numChannels,
                                         static_cast<int>(reader.bitsPerSample),
                                         reader.metadataValues,
                                         0);
    assert(writer);

    const std::unique_ptr<juce::AudioFormatWriter> firstFormatWriter(writer);
    firstFormatWriter->writeFromAudioSampleBuffer(buffer, 0, numOfSamplesToWrite);
}

void TrackManager::chooseNewNamesForSplitFiles(juce::String& firstFile, juce::String& secondFile,
                                               const juce::String& extension) const
{
    auto suffix{1u};
    while(tempClipsFolder.getChildFile(juce::StringRef(firstFile)).existsAsFile() or
          tempClipsFolder.getChildFile(juce::StringRef(secondFile)).existsAsFile())
    {
        firstFile = firstFile.replaceFirstOccurrenceOf(extension, "");
        firstFile += "(" + juce::String(suffix) + ")" + extension;
        secondFile = secondFile.replaceFirstOccurrenceOf(extension, "");
        secondFile += "(" + juce::String(suffix) + ")" + extension;
        suffix++;
    }
}

void TrackManager::addNewAudioClipsBySplit(const int trackIndex, const juce::File& fileToBeSplit,
                                           const float waveformSplitRatio, const double splitClipOffset) const
{
    juce::AudioFormatManager formatManager;
    formatManager.registerBasicFormats();
    const std::unique_ptr<juce::AudioFormatReader> reader(formatManager.createReaderFor(fileToBeSplit));
    assert(reader);

    const auto totalSamples{reader->lengthInSamples};
    const auto firstSamplesToWrite{static_cast<int>(totalSamples * waveformSplitRatio)};
    const auto secondSamplesToWrite{static_cast<int>(totalSamples - firstSamplesToWrite)};
    assert((firstSamplesToWrite + secondSamplesToWrite) == totalSamples);

    const auto splitFileExtension =
        fileToBeSplit.getFileExtension() == ".mp3" ? ".wav" : fileToBeSplit.getFileExtension();

    juce::String firstDestFileName{fileToBeSplit.getFileNameWithoutExtension() + "_part1" + splitFileExtension};
    juce::String secondDestFileName{fileToBeSplit.getFileNameWithoutExtension() + "_part2" + splitFileExtension};

    if(tempClipsFolder.getChildFile(juce::StringRef(firstDestFileName)).existsAsFile() or
       tempClipsFolder.getChildFile(juce::StringRef(secondDestFileName)).existsAsFile())
    {
        chooseNewNamesForSplitFiles(firstDestFileName, secondDestFileName, splitFileExtension);
    }

    const juce::File firstDestFile{tempClipsFolder.getFullPathName() + "/" + firstDestFileName};
    const juce::File secondDestFile{tempClipsFolder.getFullPathName() + "/" + secondDestFileName};

    handleWriteToFile(*reader, formatManager, firstDestFile, firstSamplesToWrite, 0);
    handleWriteToFile(*reader, formatManager, secondDestFile, secondSamplesToWrite, firstSamplesToWrite);

    const auto firstSplitNodeId = addAudioClipToTrack(trackIndex, firstDestFile);
    setOffsetOfAudioClipInSeconds(firstSplitNodeId, splitClipOffset);

    const auto secondSplitNodeId = addAudioClipToTrack(trackIndex, secondDestFile);
    const double secondSplitOffset{static_cast<double>(firstSamplesToWrite) / reader->sampleRate};
    setOffsetOfAudioClipInSeconds(secondSplitNodeId, splitClipOffset + secondSplitOffset);
}

/*
 * TODO: IMPORTANT! Refactor TrackManager so that it doesn't have implementation details in it!!!
 * It should delegate tasks based on ValueTree signals not actually handle them!!!
 * In particular, std::vector<std::unique_ptr<AudioTrack>> tracks should be in mainAudio or other class
 * taking care of audio handling stuff.
 * splitAudioClip To be moved to mainAudio/other audio class as well with the refactor!!!
 */
void TrackManager::splitAudioClip(const int trackIndex, const NodeID clipId, const float waveformSplitRatio) const
{
    const auto splitFile = mainAudio.getAudioClipPath(clipId);
    const auto splitClipOffset = mainAudio.getAudioClipOffsetInSeconds(clipId);
    addNewAudioClipsBySplit(trackIndex, splitFile, waveformSplitRatio, splitClipOffset);
    assert(removeAudioClipFromTrack(trackIndex, clipId));
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

void TrackManager::setTrackName(const int trackIndex, juce::String stringValue) const
{
    assert(trackIndex >= 0 && trackIndex < static_cast<int>(tracks.size()));
    tracks[trackIndex]->setTrackName(stringValue);
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
    if(property == ValueTreeIDs::soloButtonClicked)
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
    else if(property == ValueTreeIDs::trackNameChanged)
    {
        const int trackIndex = tree[ValueTreeIDs::trackNameChanged][0];
        const juce::String nameValue = tree[ValueTreeIDs::trackNameChanged][1];
        setTrackName(trackIndex, nameValue);
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
    else if(property == ValueTreeIDs::splitAudioClip)
    {
        const int trackIndex = tree[ValueTreeIDs::splitAudioClip][0];
        const int audioClipUid = tree[ValueTreeIDs::splitAudioClip][1];
        const NodeID audioClipID{static_cast<uint32_t>(audioClipUid)};
        const float waveformSplit = tree[ValueTreeIDs::splitAudioClip][2];
        splitAudioClip(trackIndex, audioClipID, waveformSplit);
    }
    else if(property == ValueTreeIDs::reorderTracks)
    {
        const int fromIndex = tree[ValueTreeIDs::reorderTracks][0];
        const int toIndex = tree[ValueTreeIDs::reorderTracks][1];
        changeTrackOrder(fromIndex, toIndex);
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
        const auto exported = exportTracksToJson();
    }
    else if(property == ValueTreeIDs::addAudioFileToNewTrack)
    {
        const juce::String audioFilePath = tree[ValueTreeIDs::addAudioFileToNewTrack];
        if(const juce::File audioFile(audioFilePath); audioFile.existsAsFile())
        {
            const int newTrackIndex = addTrack();
            std::ignore = addAudioClipToTrack(newTrackIndex, audioFile);
        }
    }
}

// TODO: maybe this shouldn't be in this class. Add this to TrackManager refactor
void TrackManager::changeTrackOrder(const int fromIndex, const int toIndex)
{
    auto trackToMove = std::move(tracks[fromIndex]);
    tracks.erase(tracks.begin() + fromIndex);
    tracks.insert(tracks.begin() + toIndex, std::move(trackToMove));
}

nlohmann::json TrackManager::exportTracksToJson() const
{
    nlohmann::json projectJson;
    projectJson["tracks"] = nlohmann::json::array();
    for(const auto& track: tracks) { projectJson["tracks"].push_back(track->toJson()); }
    return projectJson;
}

void TrackManager::clearAllTracks()
{
    if(tracks.empty())
        return;

    // Poniższe wywołania powodują błąd 0xC00...005
    trackGuiManager.clearAllTracks();
    sideMenu.clearAllTracks();
    tracks.clear();
}
