#include "Menu.h"

MenuBar::MenuBar() {
    menuBarComponent = std::make_unique<juce::MenuBarComponent>(this);
    addAndMakeVisible(menuBarComponent.get());
    // setApplicationCommandManagerToWatch(&commandManager);
    // commandManager.registerAllCommandsForTarget(this);
    setSize (500, 500);
}

void MenuBar::resized() {
    auto b = getLocalBounds();

    menuBarComponent->setBounds (b.removeFromTop (juce::LookAndFeel::getDefaultLookAndFeel()
                                                                    .getDefaultMenuBarHeight()));
}

juce::StringArray MenuBar::getMenuBarNames() {
    return menuBarNames;
}

juce::PopupMenu MenuBar::getMenuForIndex(int index, [[maybe_unused]] const juce::String& name) {
    juce::PopupMenu options;

    switch (index) {
        case File:
            break;
        case Edit:
            break;
        case View:
            break;
        case Help:
            break;
        default:
            std::unreachable();
    }

    return options;
}

void MenuBar::menuItemSelected(int menuID, int index) {}

juce::ApplicationCommandTarget* MenuBar::getNextCommandTarget() {
    return findFirstTargetParentComponent();
}

void MenuBar::getAllCommands(juce::Array<juce::CommandID>& c) {
    juce::Array<juce::CommandID> allCommands {
        NewFile,
        OpenFile,
        SaveFile,
        SaveAsFile,
        Undo,
        Redo,
        View1,
        Help1
    };
    c.addArray(allCommands);
}

void MenuBar::getCommandInfo(juce::CommandID commandID, juce::ApplicationCommandInfo& result) {
    switch (commandID) {
        case NewFile:
            result.setInfo("NewFile", "Creates a new file", "File", 0);
        case OpenFile:
            result.setInfo("OpenFile", "Opens a file", "File", 0);
        case SaveFile:
            result.setInfo("SaveFile", "Saves a file", "File", 0);
        case SaveAsFile:
            result.setInfo("SaveAsFile", "Saves as a specified file", "File", 0);
        case Undo:
            result.setInfo("Undo", "Undo a recent change", "Edit", 0);
        case Redo:
            result.setInfo("Redo", "Redo a recent change", "Edit", 0);
        case View1:
            result.setInfo("View", "Views something :)", "View", 0);
        case Help1:
            result.setInfo("Help", "Shows a help menu", "Help", 0);
        default:
            std::unreachable();
    }
}

bool MenuBar::perform(const InvocationInfo& info) {
    switch (info.commandID) {
        case NewFile:
            // what should clicking NewFile do? etc.
            break;
        case OpenFile:
            break;
        case SaveFile:
            break;
        case SaveAsFile:
            break;
        case Undo:
            break;
        case Redo:
            break;
        case View1:
            break;
        case Help1:
            break;
        default:
            return false;
    }
    return true;
}