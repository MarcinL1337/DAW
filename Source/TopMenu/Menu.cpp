#include "Menu.h"

#include "../Constants.h"

Menu::Menu(const juce::ValueTree& parentTree) : tree{parentTree}
{
    menuBarComponent = std::make_unique<juce::MenuBarComponent>(this);
    addAndMakeVisible(menuBarComponent.get());
    setApplicationCommandManagerToWatch(&commandManager);
    commandManager.registerAllCommandsForTarget(this);
    commandManager.setFirstCommandTarget(this);
}

Menu::~Menu() { commandManager.setFirstCommandTarget(nullptr); }

void Menu::resized() { menuBarComponent->setBounds(getLocalBounds()); }

void Menu::paint(juce::Graphics& g)
{
    g.setColour(juce::Colours::black);
    g.drawRect(getLocalBounds());
}

juce::StringArray Menu::getMenuBarNames() { return menuBarNames; }

juce::PopupMenu Menu::getMenuForIndex(const int index, [[maybe_unused]] const juce::String& name)
{
    juce::PopupMenu options;

    switch(index)
    {
        case file:
            options.addCommandItem(&commandManager, newFile);
            options.addCommandItem(&commandManager, openFile);
            options.addCommandItem(&commandManager, saveFile);
            options.addCommandItem(&commandManager, saveAsFile);
            break;
        case edit:
            options.addCommandItem(&commandManager, undo);
            options.addCommandItem(&commandManager, redo);
            break;
        case view:
            options.addCommandItem(&commandManager, view1);
            break;
        case help:
            options.addCommandItem(&commandManager, help1);
            break;
        default:
            std::unreachable();
    }

    return options;
}

void Menu::menuItemSelected([[maybe_unused]] int menuID, [[maybe_unused]] int index) {}

juce::ApplicationCommandTarget* Menu::getNextCommandTarget() { return findFirstTargetParentComponent(); }

void Menu::getAllCommands(juce::Array<juce::CommandID>& c)
{
    const juce::Array<juce::CommandID> allCommands{newFile, openFile, saveFile, saveAsFile, undo, redo, view1, help1};
    c.addArray(allCommands);
}

void Menu::getCommandInfo(const juce::CommandID commandID, juce::ApplicationCommandInfo& result)
{
    switch(commandID)
    {
        case newFile:
            result.setInfo("NewFile", "Creates a new file", "File", 0);
            result.addDefaultKeypress('n', juce::ModifierKeys::ctrlModifier);
            break;
        case openFile:
            result.setInfo("OpenFile", "Opens a file", "File", 0);
            result.addDefaultKeypress('o', juce::ModifierKeys::ctrlModifier);
            break;
        case saveFile:
            result.setInfo("SaveFile", "Saves a file", "File", 0);
            result.addDefaultKeypress('s', juce::ModifierKeys::ctrlModifier);
            break;
        case saveAsFile:
            result.setInfo("SaveAsFile", "Saves as a specified file", "File", 0);
            result.addDefaultKeypress('s', juce::ModifierKeys::shiftModifier | juce::ModifierKeys::ctrlModifier);
            break;
        case undo:
            result.setInfo("Undo", "Undo a recent change", "Edit", 0);
            result.addDefaultKeypress('z', juce::ModifierKeys::ctrlModifier);
            break;
        case redo:
            result.setInfo("Redo", "Redo a recent change", "Edit", 0);
            result.addDefaultKeypress('y', juce::ModifierKeys::ctrlModifier);
            break;
        case view1:
            result.setInfo("View", "Views something :)", "View", 0);
            break;
        case help1:
            result.setInfo("Help", "Shows a help menu", "Help", 0);
            result.addDefaultKeypress('h', juce::ModifierKeys::ctrlModifier);
            break;
        default:
            std::unreachable();
    }
}

bool Menu::perform(const InvocationInfo& info)
{
    switch(info.commandID)
    {
        case newFile:
            break;
        case openFile:
            openFileButtonClicked();
            break;
        case saveFile:
            break;
        case saveAsFile:
            break;
        case undo:
            break;
        case redo:
            break;
        case view1:
            break;
        case help1:
            break;
        default:
            return false;
    }
    return true;
}

void Menu::openFileButtonClicked()
{
    const auto folderChooserFlags = juce::FileBrowserComponent::openMode | juce::FileBrowserComponent::canSelectFiles |
                                    juce::FileBrowserComponent::canSelectDirectories;
    // TODO: when choosing to open the same file twice, the value in the ValueTree stays the same, meaning no update is
    // propagated which results in not adding the file for the second time. Fix it.
    fileChooser.launchAsync(folderChooserFlags,
                            [this](const juce::FileChooser& chooser)
                            {
                                auto selectedFileFullPath{chooser.getResult().getFullPathName()};
                                if(selectedFileFullPath.isNotEmpty())
                                {
                                    tree.setProperty(newAudioFile, ValueTreeConstants::doNothing, nullptr);
                                    tree.setProperty(newAudioFile, selectedFileFullPath, nullptr);
                                }
                            });
}
