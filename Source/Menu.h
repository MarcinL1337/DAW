#pragma once

#include <juce_gui_extra/juce_gui_extra.h>

class MenuBar : public juce::Component,
                public juce::ApplicationCommandTarget,
                public juce::MenuBarModel
{
public:
    MenuBar();
    ~MenuBar() override = default;


private:
    juce::StringArray getMenuBarNames() override;
    juce::PopupMenu getMenuForIndex(int index, const juce::String& name) override;
    void menuItemSelected(int menuID, int index) override;
    ApplicationCommandTarget* getNextCommandTarget() override;
    void getAllCommands(juce::Array<juce::CommandID>& c) override;
    void getCommandInfo(juce::CommandID commandID, juce::ApplicationCommandInfo& result) override;
    bool perform(const InvocationInfo& info) override;

    juce::MenuBarComponent menuComponent{};
    juce::StringArray menuBarNames{"File", "Edit", "View", "Help" };

    enum TopLevelMenuOptions {
        File = 0,
        Edit,
        View,
        Help
    };

    enum FileMenuOptions {
        NewFile = 0,
        OpenFile,
        SaveFile,
        SaveAsFile
    };

    enum EditMenuOptions {
        Edit1 = 0
    };

    enum ViewMenuOptions {
        View1 = 0
    };

    enum HelpMenuOptions {
        Help1 = 0
    };

};