#include "Menu.h"

#include "../Constants.h"

Menu::Menu(juce::ValueTree& parentTree) : tree{parentTree}, keyPressMappingSet{commandManager}
{
    menuBarComponent = std::make_unique<juce::MenuBarComponent>(this);
    addAndMakeVisible(menuBarComponent.get());
    setApplicationCommandManagerToWatch(&commandManager);
    commandManager.registerAllCommandsForTarget(this);
    commandManager.setFirstCommandTarget(this);

    setWantsKeyboardFocus(true);
    keyPressMappingSet.addKeyPress(openFile, juce::KeyPress('o', juce::ModifierKeys::ctrlModifier, 0));
}

Menu::~Menu() { commandManager.setFirstCommandTarget(nullptr); }

void Menu::resized() { menuBarComponent->setBounds(getLocalBounds()); }

void Menu::paint(juce::Graphics& g)
{
    g.setColour(juce::Colours::black);
    g.drawRect(getLocalBounds());
}

bool Menu::keyPressed(const juce::KeyPress& key)
{
    // Let the KeyPressMappingSet handle the key press
    // It will check if this key matches any registered shortcuts
    // and trigger the appropriate command if it does
    std::cerr << "Key pressed: " << key.getTextDescription();
    return keyPressMappingSet.keyPressed(key, this);
}

juce::StringArray Menu::getMenuBarNames() { return menuBarNames; }

juce::PopupMenu Menu::getMenuForIndex(const int index, [[maybe_unused]] const juce::String& name)
{
    juce::PopupMenu options;

    switch(index)
    {
        case file:
            options.addCommandItem(&commandManager, openFile);
            break;
        case help:
            options.addCommandItem(&commandManager, howToUse);
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
    const juce::Array<juce::CommandID> allCommands{openFile, howToUse};
    c.addArray(allCommands);
}

void Menu::getCommandInfo(const juce::CommandID commandID, juce::ApplicationCommandInfo& result)
{
    switch(commandID)
    {
        case openFile:
            result.setInfo("Open a file", "Opens a file", "File", 0);
            result.addDefaultKeypress('o', juce::ModifierKeys::ctrlModifier);
            break;
        case howToUse:
            result.setInfo("How to use", "Shows a help menu", "Help", 0);
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
        case openFile:
            openFileButtonClicked();
            break;
        case howToUse:
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
                                    tree.setProperty(
                                        ValueTreeIDs::newAudioFile, ValueTreeConstants::doNothing, nullptr);
                                    tree.setProperty(ValueTreeIDs::newAudioFile, selectedFileFullPath, nullptr);
                                }
                            });
}
