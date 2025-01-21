#pragma once

#include <juce_gui_extra/juce_gui_extra.h>
#include <utility>


class MenuBar : public juce::Component,
                public juce::ApplicationCommandTarget,
                public juce::MenuBarModel
{
public:
    MenuBar();
    ~MenuBar() override = default;


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
        File = 0,
        Edit,
        View,
        Help
    };

    // TODO: Better idea of representation instead of comments like that
    enum LowerLevelMenuOptions {
        //File
        NewFile = 0,
        OpenFile,
        SaveFile,
        SaveAsFile,
        //Edit
        Undo,
        Redo,
        //View
        View1,
        //Help
        Help1
    };
};