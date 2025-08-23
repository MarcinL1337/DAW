#pragma once
#include <juce_gui_extra/juce_gui_extra.h>
#include <nlohmann/json.hpp>

class ProjectFilesManager final : public juce::ValueTree::Listener
{
public:
    explicit ProjectFilesManager(juce::ValueTree& parentTree);
    ~ProjectFilesManager() override;

    void createNewProject();
    void openProject();
    void saveProject();
    void saveAsProject();
    void addAudioFile();
    void openTestProject();

private:
    void markAsDirty();
    void markAsClean();
    void saveProjectToFile(const juce::File& file) const;
    void loadProjectFromFile(const juce::File& file);
    void createNewProjectInternal();
    void openProjectInternal();
    void cleanupUnusedAudioFiles(const juce::String& projectJsonString) const;
    void valueTreePropertyChanged(juce::ValueTree& treeWhosePropertyHasChanged,
                                  const juce::Identifier& property) override;
    juce::File getTempAudioDirectory() const;
    void cleanupTempDirectory() const;

    bool isDirty{false};
    juce::ValueTree& tree;
    juce::File currentProjectFile{};
    static constexpr auto audioDirSuffix = "AudioFiles";

    juce::FileChooser saveProjectChooser{
        "", juce::File::getSpecialLocation(juce::File::userDocumentsDirectory), "*.json"};

    juce::FileChooser projectFileChooser{
        "Choose project file to open", juce::File::getSpecialLocation(juce::File::userDocumentsDirectory), "*.json"};

    std::string fileChooserDialogText{"Choose an audio file to open"};
    juce::File fileChooserStartDirectory{juce::File::getSpecialLocation(juce::File::userMusicDirectory)};
    std::string fileChooserValidFileExtensions{"*.wav;*.mp3"};
    juce::FileChooser audioFileChooser{
        fileChooserDialogText, fileChooserStartDirectory, fileChooserValidFileExtensions};
};
