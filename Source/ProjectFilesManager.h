#pragma once
#include <juce_gui_extra/juce_gui_extra.h>
#include <nlohmann/json.hpp>

#include "TrackManager.h"

class ProjectFilesManager final
{
public:
    explicit ProjectFilesManager(TrackManager& trackManagerRef);

    void createNewProject();
    void openProject();
    void saveProject();
    void saveAsProject();
    void addAudioFile();
    void openTestProject();

private:
    void saveProjectToFile(const juce::File& file) const;
    void loadProjectFromFile(const juce::File& file) const;

    TrackManager& trackManager;
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
