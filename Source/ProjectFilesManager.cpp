#include "ProjectFilesManager.h"
#include "Constants.h"

ProjectFilesManager::ProjectFilesManager(juce::ValueTree& parentTree) : tree{parentTree}
{
    tree.addListener(this);
    cleanupTempDirectory();
    markAsClean();
}

ProjectFilesManager::~ProjectFilesManager() { cleanupTempDirectory(); }

void ProjectFilesManager::createNewProject()
{
    tree.setProperty(ValueTreeIDs::clearAllTracks, true, nullptr);
    tree.setProperty(ValueTreeIDs::clearAllTracks, ValueTreeConstants::doNothing, nullptr);
    currentProjectFile = juce::File{};
    markAsClean();
}

void ProjectFilesManager::openProject()
{
    constexpr auto flags = juce::FileBrowserComponent::openMode | juce::FileBrowserComponent::canSelectFiles;

    projectFileChooser.launchAsync(flags,
                                   [this](const juce::FileChooser& chooser)
                                   {
                                       if(const auto file = chooser.getResult(); file.existsAsFile())
                                       {
                                           loadProjectFromFile(file);
                                           currentProjectFile = file;
                                       }
                                   });
}

void ProjectFilesManager::saveProject()
{
    if(currentProjectFile.existsAsFile())
        saveProjectToFile(currentProjectFile);
    else
        saveAsProject();
}

void ProjectFilesManager::saveAsProject()
{
    constexpr auto flags = juce::FileBrowserComponent::saveMode | juce::FileBrowserComponent::canSelectFiles;

    saveProjectChooser.launchAsync(flags,
                                   [this](const juce::FileChooser& chooser)
                                   {
                                       if(const auto file = chooser.getResult(); file.getParentDirectory().exists())
                                       {
                                           currentProjectFile = file;
                                           saveProjectToFile(file);
                                       }
                                   });
}

void ProjectFilesManager::saveProjectToFile(const juce::File& file) const
{
    const juce::File projectDir = file.getParentDirectory();
    const juce::File audioDir = projectDir.getChildFile(file.getFileNameWithoutExtension() + audioDirSuffix);

    if(tree.hasProperty(ValueTreeIDs::projectAudioDir))
    {
        const juce::File currentAudioDir{tree[ValueTreeIDs::projectAudioDir].toString()};
        if(currentAudioDir.exists() && currentAudioDir != audioDir)
        {
            const bool result = currentAudioDir.copyDirectoryTo(audioDir);
            assert(result);
        }
    }

    if(!audioDir.exists())
    {
        const auto result = audioDir.createDirectory();
        assert(result.wasOk());
    }

    tree.setProperty(ValueTreeIDs::projectAudioDir, audioDir.getFullPathName(), nullptr);
    tree.setProperty(ValueTreeIDs::exportTracksToJson, true, nullptr);
    tree.setProperty(ValueTreeIDs::exportTracksToJson, ValueTreeConstants::doNothing, nullptr);
}

void ProjectFilesManager::loadProjectFromFile(const juce::File& file)
{
    tree.setProperty(ValueTreeIDs::clearAllTracks, true, nullptr);
    tree.setProperty(ValueTreeIDs::clearAllTracks, ValueTreeConstants::doNothing, nullptr);

    const auto jsonString = file.loadFileAsString();
    auto projectJson = nlohmann::json::parse(jsonString.toStdString());
    const juce::File audioDir =
        file.getParentDirectory().getChildFile(file.getFileNameWithoutExtension() + audioDirSuffix);
    tree.setProperty(ValueTreeIDs::projectAudioDir, audioDir.getFullPathName(), nullptr);

    if(not projectJson.contains("tracks"))
        return;

    for(auto& trackJson: projectJson["tracks"])
    {
        for(auto& clip: trackJson["audioClips"])
        {
            juce::File audioFile = audioDir.getChildFile(clip["path"].get<std::string>());
            clip["path"] = audioFile.getFullPathName().toStdString();
        }

        juce::String trackJsonString = trackJson.dump();
        tree.setProperty(ValueTreeIDs::createTrackFromJson, trackJsonString, nullptr);
        tree.setProperty(ValueTreeIDs::createTrackFromJson, ValueTreeConstants::doNothing, nullptr);
    }
    juce::Timer::callAfterDelay(500, [this]() { markAsClean(); });
}

void ProjectFilesManager::addAudioFile()
{
    static long long counter = 1;
    constexpr auto folderChooserFlags = juce::FileBrowserComponent::openMode |
                                        juce::FileBrowserComponent::canSelectFiles |
                                        juce::FileBrowserComponent::canSelectDirectories;
    audioFileChooser.launchAsync(
        folderChooserFlags,
        [this](const juce::FileChooser& chooser)
        {
            if(const auto file = chooser.getResult(); file.existsAsFile())
            {
                juce::File audioDir;

                if(currentProjectFile.existsAsFile())
                {
                    const auto currentProjectDir = currentProjectFile.getParentDirectory();
                    audioDir = currentProjectDir.getChildFile(currentProjectFile.getFileNameWithoutExtension() +
                                                              audioDirSuffix);
                }
                else
                {
                    audioDir = getTempAudioDirectory();
                    tree.setProperty(ValueTreeIDs::projectAudioDir, audioDir.getFullPathName(), nullptr);
                }

                const juce::File dstFile = audioDir.getChildFile(file.getFileNameWithoutExtension() +
                                                                 std::to_string(counter++) + file.getFileExtension());
                if(!dstFile.existsAsFile())
                {
                    const auto result = file.copyFileTo(dstFile);
                    assert(result);
                }

                tree.setProperty(ValueTreeIDs::addAudioFileToNewTrack, dstFile.getFullPathName(), nullptr);
                tree.setProperty(ValueTreeIDs::addAudioFileToNewTrack, ValueTreeConstants::doNothing, nullptr);
            }
        });
}

void ProjectFilesManager::openTestProject()
{
    const juce::File dawDir =
        juce::File::getCurrentWorkingDirectory().getParentDirectory().getParentDirectory().getParentDirectory();
    const juce::File projectFile = dawDir.getChildFile("Assets/test_project/test_project.json");
    currentProjectFile = projectFile;
    loadProjectFromFile(projectFile);
}

void ProjectFilesManager::valueTreePropertyChanged(juce::ValueTree&, const juce::Identifier& property)
{
    if(static_cast<int>(tree[property]) == ValueTreeConstants::doNothing)
        return;

    if(property == ValueTreeIDs::createNewProject)
        createNewProject();
    else if(property == ValueTreeIDs::openProject)
        openProject();
    else if(property == ValueTreeIDs::saveProject)
        saveProject();
    else if(property == ValueTreeIDs::saveAsProject)
        saveAsProject();
    else if(property == ValueTreeIDs::addAudioFile)
        addAudioFile();
    else if(property == ValueTreeIDs::tracksJsonExported)
    {
        const auto projectString = tree[ValueTreeIDs::tracksJsonExported].toString();
        const auto result = currentProjectFile.replaceWithText(projectString);
        assert(result);
        markAsClean();
        cleanupUnusedAudioFiles(projectString);
    }
    else if(property == ValueTreeIDs::trackGainChanged || property == ValueTreeIDs::trackPanChanged ||
            property == ValueTreeIDs::trackReverbChanged || property == ValueTreeIDs::trackNameChanged ||
            property == ValueTreeIDs::deleteTrackGui || property == ValueTreeIDs::duplicateTrackGui ||
            property == ValueTreeIDs::deleteAudioClip || property == ValueTreeIDs::splitAudioClip ||
            property == ValueTreeIDs::reorderTracks || property == ValueTreeIDs::audioClipFadeChanged ||
            property == ValueTreeIDs::addAudioFileToNewTrack || property == ValueTreeIDs::trackRoomSizeChanged ||
            property == ValueTreeIDs::trackDampChanged || property == ValueTreeIDs::trackWetLevelChanged ||
            property == ValueTreeIDs::trackDryLevelChanged || property == ValueTreeIDs::trackWidthChanged ||
            property == ValueTreeIDs::trackFreezeChanged || property == ValueTreeIDs::pasteAudioClip ||
            property == ValueTreeIDs::muteButtonClicked || property == ValueTreeIDs::soloButtonClicked)
    {
        markAsDirty();
    }
}

void ProjectFilesManager::markAsDirty()
{
    if(!isDirty)
    {
        isDirty = true;
        tree.setProperty(ValueTreeIDs::projectDirtyStateChanged, true, nullptr);
    }
}

void ProjectFilesManager::markAsClean()
{
    if(isDirty)
    {
        isDirty = false;
        tree.setProperty(ValueTreeIDs::projectDirtyStateChanged, false, nullptr);
    }
}

void ProjectFilesManager::cleanupUnusedAudioFiles(const juce::String& projectJsonString) const
{
    if(!currentProjectFile.existsAsFile())
        return;

    const juce::File audioDir = currentProjectFile.getParentDirectory().getChildFile(
        currentProjectFile.getFileNameWithoutExtension() + audioDirSuffix);

    if(!audioDir.exists())
        return;

    juce::StringArray usedFiles;
    auto projectJson = nlohmann::json::parse(projectJsonString.toStdString());

    if(projectJson.contains("tracks"))
        for(const auto& track: projectJson["tracks"])
            if(track.contains("audioClips"))
                for(const auto& clip: track["audioClips"])
                    if(clip.contains("path"))
                        usedFiles.add(clip["path"].get<std::string>());

    juce::Array<juce::File> audioFiles = audioDir.findChildFiles(juce::File::findFiles, false);
    for(const auto& file: audioFiles)
        if(!usedFiles.contains(file.getFileName()))
        {
            const bool result = file.deleteFile();
            assert(result);
        }
}

juce::File ProjectFilesManager::getTempAudioDirectory() const
{
    auto tempDir = juce::File::getSpecialLocation(juce::File::tempDirectory).getChildFile("DAW_AudioFiles");

    if(!tempDir.exists())
    {
        const auto result = tempDir.createDirectory();
        assert(result.wasOk());
    }
    std::cout << tempDir.getFullPathName() << " " << tempDir.exists() << std::endl;
    return tempDir;
}

void ProjectFilesManager::cleanupTempDirectory() const
{
    const bool result = getTempAudioDirectory().deleteRecursively();
    assert(result);
}