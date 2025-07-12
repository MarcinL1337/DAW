#include "ProjectFilesManager.h"

ProjectFilesManager::ProjectFilesManager(TrackManager& trackManagerRef) : trackManager{trackManagerRef} {}

void ProjectFilesManager::createNewProject()
{
    trackManager.clearAllTracks();
    currentProjectFile = juce::File{};
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
    if(currentProjectFile != juce::File{})
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
                                           saveProjectToFile(file);
                                           currentProjectFile = file;
                                       }
                                   });
}

void ProjectFilesManager::saveProjectToFile(const juce::File& file) const
{
    const juce::File projectDir = file.getParentDirectory();
    const juce::File audioDir = projectDir.getChildFile(file.getFileNameWithoutExtension() + audioDirSuffix);
    if(!audioDir.exists()) [[maybe_unused]]
        auto result = audioDir.createDirectory();

    const auto projectJson = trackManager.exportTracksToJson();
    [[maybe_unused]] auto result = file.replaceWithText(projectJson.dump(4));
}

void ProjectFilesManager::loadProjectFromFile(const juce::File& file) const
{
    trackManager.clearAllTracks();

    const auto jsonString = file.loadFileAsString();
    auto projectJson = nlohmann::json::parse(jsonString.toStdString());
    const juce::File audioDir =
        file.getParentDirectory().getChildFile(file.getFileNameWithoutExtension() + audioDirSuffix);
    if(not projectJson.contains("tracks"))
        return;

    for(auto& trackJson: projectJson["tracks"])
    {
        for(auto& clip: trackJson["audioClips"])
        {
            juce::File audioFile = audioDir.getChildFile(clip["path"].get<std::string>());
            clip["path"] = audioFile.getFullPathName().toStdString();
        }
        trackManager.createTrackFromJson(trackJson);
    }
}

void ProjectFilesManager::addAudioFile()
{
    if(not currentProjectFile.existsAsFile() or currentProjectFile == juce::File{})
    {
        const auto options =
            juce::MessageBoxOptions()
                .withIconType(juce::MessageBoxIconType::QuestionIcon)
                .withTitle("Project not saved")
                .withMessage(
                    "To add an audio file, the project must be saved first. Do you want to save the project now?")
                .withButton("Save")
                .withButton("Cancel");

        juce::AlertWindow::showAsync(options,
                                     [this](const int result)
                                     {
                                         if(result == 1)
                                             saveAsProject();
                                     });
        return;
    }

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
                const auto currentProjectDir = currentProjectFile.getParentDirectory();
                const juce::File audioDir =
                    currentProjectDir.getChildFile(currentProjectFile.getFileNameWithoutExtension() + audioDirSuffix);

                if(!audioDir.exists()) [[maybe_unused]]
                    auto result = audioDir.createDirectory();

                const juce::File dstFile = audioDir.getChildFile(file.getFileNameWithoutExtension() +
                                                                 std::to_string(counter++) + file.getFileExtension());
                if(!dstFile.existsAsFile()) [[maybe_unused]]
                    auto result = file.copyFileTo(dstFile);

                const auto index = trackManager.addTrack();
                [[maybe_unused]] const auto nodeId = trackManager.addAudioClipToTrack(index, dstFile.getFullPathName());
                saveProjectToFile(currentProjectFile);
            }
        });
}

void ProjectFilesManager::openTestProject()
{
    const juce::File dawDir =
        juce::File::getCurrentWorkingDirectory().getParentDirectory().getParentDirectory().getParentDirectory();
    const juce::File projectFile = dawDir.getChildFile("Assets/test_project/test_project.json");
    loadProjectFromFile(projectFile);
    currentProjectFile = projectFile;
}
