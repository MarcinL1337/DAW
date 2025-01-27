#pragma once

#include <juce_gui_extra/juce_gui_extra.h>


class Menu final : public juce::Component,
                   public juce::ApplicationCommandTarget,
                   public juce::MenuBarModel
{
public:
    Menu();
    ~Menu() override;

    auto getMenuHeight() { return menuHeight; }
    auto getMenuWidth() { return menuWidth; }
    auto getXPosition() { return xPosition; }
    auto getYPosition() { return yPosition; }


private:
    void resized() override;

    juce::StringArray getMenuBarNames() override;
    juce::PopupMenu getMenuForIndex(int index, const juce::String& name) override;
    void menuItemSelected(int menuID, int index) override;
    ApplicationCommandTarget* getNextCommandTarget() override;
    void getAllCommands(juce::Array<juce::CommandID>& c) override;
    void getCommandInfo(juce::CommandID commandID, juce::ApplicationCommandInfo& result) override;
    bool perform(const InvocationInfo& info) override;

    std::unique_ptr<juce::MenuBarComponent> menuBarComponent;
    juce::ApplicationCommandManager commandManager;

    juce::StringArray menuBarNames{"File", "Edit", "View", "Help" };

    enum TopLevelMenuOptions {
        file = 0,
        edit,
        view,
        help
    };

    // TODO: Better idea of representation instead of comments like that
    enum LowerLevelMenuOptions {
        //File
        newFile = 1,
        openFile,
        saveFile,
        saveAsFile,
        //Edit
        undo,
        redo,
        //View
        view1,
        //Help
        help1
    };

    const int menuHeight{50};
    const int menuWidth{getWidth()};
    const int xPosition{0};
    const int yPosition{0};
};