#include "Menu.h"

Menu::Menu() {
    menuBarComponent = std::make_unique<juce::MenuBarComponent>(this);
    addAndMakeVisible(menuBarComponent.get());
    setApplicationCommandManagerToWatch(&commandManager);
    commandManager.registerAllCommandsForTarget(this);
    commandManager.setFirstCommandTarget (this);
    setSize (500, 500);
}

Menu::~Menu() {
    commandManager.setFirstCommandTarget (nullptr);
}

void Menu::resized() {
    auto localBounds = getLocalBounds();

    menuBarComponent->setBounds (localBounds.removeFromTop (juce::LookAndFeel::getDefaultLookAndFeel().getDefaultMenuBarHeight()));
}

juce::StringArray Menu::getMenuBarNames() {
    return menuBarNames;
}

juce::PopupMenu Menu::getMenuForIndex(int index, [[maybe_unused]] const juce::String& name) {
    juce::PopupMenu options;

    switch (index) {
        case File:
            options.addCommandItem(&commandManager, NewFile);
            options.addCommandItem(&commandManager, OpenFile);
            options.addCommandItem(&commandManager, SaveFile);
            options.addCommandItem(&commandManager, SaveAsFile);
            break;
        case Edit:
            options.addCommandItem(&commandManager, Undo);
            options.addCommandItem(&commandManager, Redo);
            break;
        case View:
            options.addCommandItem(&commandManager, View1);
            break;
        case Help:
            options.addCommandItem(&commandManager, Help1);
            break;
        default:
            std::unreachable();
    }

    return options;
}

void Menu::menuItemSelected([[maybe_unused]] int menuID, [[maybe_unused]] int index) {}

juce::ApplicationCommandTarget* Menu::getNextCommandTarget() {
    return findFirstTargetParentComponent();
}

void Menu::getAllCommands(juce::Array<juce::CommandID>& c) {
    const juce::Array<juce::CommandID> allCommands {
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

void Menu::getCommandInfo(juce::CommandID commandID, juce::ApplicationCommandInfo& result) {
    switch (commandID) {
        case NewFile:
            result.setInfo("NewFile", "Creates a new file", "File", 0);
            result.addDefaultKeypress ('n', juce::ModifierKeys::ctrlModifier);
            break;
        case OpenFile:
            result.setInfo("OpenFile", "Opens a file", "File", 0);
            result.addDefaultKeypress ('o', juce::ModifierKeys::ctrlModifier);
            break;
        case SaveFile:
            result.setInfo("SaveFile", "Saves a file", "File", 0);
            result.addDefaultKeypress ('s', juce::ModifierKeys::ctrlModifier);
            break;
        case SaveAsFile:
            result.setInfo("SaveAsFile", "Saves as a specified file", "File", 0);
            result.addDefaultKeypress ('s', juce::ModifierKeys::shiftModifier | juce::ModifierKeys::ctrlModifier);
            break;
        case Undo:
            result.setInfo("Undo", "Undo a recent change", "Edit", 0);
            result.addDefaultKeypress ('z', juce::ModifierKeys::ctrlModifier);
            break;
        case Redo:
            result.setInfo("Redo", "Redo a recent change", "Edit", 0);
            result.addDefaultKeypress ('y', juce::ModifierKeys::ctrlModifier);
            break;
        case View1:
            result.setInfo("View", "Views something :)", "View", 0);
            break;
        case Help1:
            result.setInfo("Help", "Shows a help menu", "Help", 0);
            result.addDefaultKeypress ('h', juce::ModifierKeys::ctrlModifier);
            break;
        default:
            std::unreachable();
    }
}

bool Menu::perform(const InvocationInfo& info) {
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