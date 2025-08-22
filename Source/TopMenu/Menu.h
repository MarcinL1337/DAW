#pragma once

#include <juce_gui_extra/juce_gui_extra.h>

class Menu final : public juce::Component,
                   public juce::ApplicationCommandTarget,
                   public juce::MenuBarModel
{
public:
    explicit Menu(juce::ValueTree& parentTree);
    ~Menu() override;

    bool keyPressed(const juce::KeyPress& key) override;

private:
    void resized() override;
    void paint(juce::Graphics& g) override;

    juce::StringArray getMenuBarNames() override;
    juce::PopupMenu getMenuForIndex(int index, const juce::String& name) override;
    void menuItemSelected(int menuID, int index) override;
    ApplicationCommandTarget* getNextCommandTarget() override;
    void getAllCommands(juce::Array<juce::CommandID>& c) override;
    void getCommandInfo(juce::CommandID commandID, juce::ApplicationCommandInfo& result) override;
    bool perform(const InvocationInfo& info) override;

    void setKeyMapping();
    void openFileButtonClicked();
    void openHelp();

    std::unique_ptr<juce::MenuBarComponent> menuBarComponent;
    juce::ApplicationCommandManager commandManager;
    juce::KeyPressMappingSet keyPressMappingSet;
    juce::ValueTree& tree;

    std::string fileChooserDialogText{"Choose an audio file to open"};
    juce::File fileChooserStartDirectory{juce::File::getSpecialLocation(juce::File::userMusicDirectory)};
    std::string fileChooserValidFileExtensions{"*.wav;*.mp3"};
    juce::FileChooser fileChooser{fileChooserDialogText, fileChooserStartDirectory, fileChooserValidFileExtensions};

    juce::StringArray menuBarNames{"File", "Help"};

    enum TopLevelMenuOptions
    {
        file = 0,
        help
    };

    enum LowerLevelMenuOptions
    {
        // File
        openFile = 1,
        // Help
        howToUse
    };
};