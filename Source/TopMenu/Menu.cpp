#include "Menu.h"

#include "../Constants.h"

Menu::Menu(juce::ValueTree& parentTree) : keyPressMappingSet{commandManager}, tree{parentTree}
{
    menuBarComponent = std::make_unique<juce::MenuBarComponent>(this);
    addAndMakeVisible(menuBarComponent.get());
    setApplicationCommandManagerToWatch(&commandManager);
    commandManager.registerAllCommandsForTarget(this);
    commandManager.setFirstCommandTarget(this);

    setKeyMapping();
}

Menu::~Menu() { commandManager.setFirstCommandTarget(nullptr); }

void Menu::resized() { menuBarComponent->setBounds(getLocalBounds()); }

void Menu::paint(juce::Graphics& g)
{
    g.setColour(juce::Colours::black);
    g.drawRect(getLocalBounds());
}

void Menu::setKeyMapping()
{
    keyPressMappingSet.addKeyPress(newProject, juce::KeyPress('n', juce::ModifierKeys::ctrlModifier, 0));
    keyPressMappingSet.addKeyPress(openProject, juce::KeyPress('o', juce::ModifierKeys::ctrlModifier, 0));
    keyPressMappingSet.addKeyPress(saveProject, juce::KeyPress('s', juce::ModifierKeys::ctrlModifier, 0));
    keyPressMappingSet.addKeyPress(
        saveProjectAs, juce::KeyPress('s', juce::ModifierKeys::shiftModifier | juce::ModifierKeys::ctrlModifier, 0));
    keyPressMappingSet.addKeyPress(
        addAudioFile, juce::KeyPress('a', juce::ModifierKeys::shiftModifier | juce::ModifierKeys::ctrlModifier, 0));
    keyPressMappingSet.addKeyPress(
        exportToWav, juce::KeyPress('e', juce::ModifierKeys::shiftModifier | juce::ModifierKeys::ctrlModifier, 0));
    keyPressMappingSet.addKeyPress(howToUse, juce::KeyPress('h', juce::ModifierKeys::ctrlModifier, 0));
}

bool Menu::keyPressed(const juce::KeyPress& key) { return keyPressMappingSet.keyPressed(key, this); }

juce::StringArray Menu::getMenuBarNames() { return menuBarNames; }

juce::PopupMenu Menu::getMenuForIndex(const int index, [[maybe_unused]] const juce::String& name)
{
    juce::PopupMenu options;

    switch(index)
    {
        case file:
            options.addCommandItem(&commandManager, newProject);
            options.addCommandItem(&commandManager, openProject);
            options.addCommandItem(&commandManager, saveProject);
            options.addCommandItem(&commandManager, saveProjectAs);
            options.addCommandItem(&commandManager, addAudioFile);
            options.addCommandItem(&commandManager, exportToWav);
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
    const juce::Array<juce::CommandID> allCommands{
        newProject, openProject, saveProject, saveProjectAs, addAudioFile, exportToWav, howToUse};
    c.addArray(allCommands);
}

void Menu::getCommandInfo(const juce::CommandID commandID, juce::ApplicationCommandInfo& result)
{
    switch(commandID)
    {
        case newProject:
            result.setInfo("New project", "Creates a new project", "File", 0);
            result.addDefaultKeypress('n', juce::ModifierKeys::ctrlModifier);
            break;
        case openProject:
            result.setInfo("Open project", "Opens an existing project", "File", 0);
            result.addDefaultKeypress('o', juce::ModifierKeys::ctrlModifier);
            break;
        case saveProject:
            result.setInfo("Save project", "Saves current project", "File", 0);
            result.addDefaultKeypress('s', juce::ModifierKeys::ctrlModifier);
            break;
        case saveProjectAs:
            result.setInfo("Save project as", "Saves current project as", "File", 0);
            result.addDefaultKeypress('s', juce::ModifierKeys::shiftModifier | juce::ModifierKeys::ctrlModifier);
            break;
        case addAudioFile:
            result.setInfo("Add audio file", "Adds new audio file to project", "File", 0);
            result.addDefaultKeypress('a', juce::ModifierKeys::shiftModifier | juce::ModifierKeys::ctrlModifier);
            break;
        case exportToWav:
            result.setInfo("Export to wav", "Exports project to wav", "File", 0);
            result.addDefaultKeypress('e', juce::ModifierKeys::shiftModifier | juce::ModifierKeys::ctrlModifier);
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
        case newProject:
            tree.setProperty(ValueTreeIDs::createNewProject, true, nullptr);
            tree.setProperty(ValueTreeIDs::createNewProject, ValueTreeConstants::doNothing, nullptr);
            break;
        case openProject:
            tree.setProperty(ValueTreeIDs::openProject, true, nullptr);
            tree.setProperty(ValueTreeIDs::openProject, ValueTreeConstants::doNothing, nullptr);
            break;
        case saveProject:
            tree.setProperty(ValueTreeIDs::saveProject, true, nullptr);
            tree.setProperty(ValueTreeIDs::saveProject, ValueTreeConstants::doNothing, nullptr);
            break;
        case saveProjectAs:
            tree.setProperty(ValueTreeIDs::saveAsProject, true, nullptr);
            tree.setProperty(ValueTreeIDs::saveAsProject, ValueTreeConstants::doNothing, nullptr);
            break;
        case addAudioFile:
            tree.setProperty(ValueTreeIDs::addAudioFile, true, nullptr);
            tree.setProperty(ValueTreeIDs::addAudioFile, ValueTreeConstants::doNothing, nullptr);
            break;
        case exportToWav:
            tree.setProperty(ValueTreeIDs::exportToWav, true, nullptr);
            tree.setProperty(ValueTreeIDs::exportToWav, ValueTreeConstants::doNothing, nullptr);
            break;
        case howToUse:
            openHelp();
            break;
        default:
            return false;
    }
    return true;
}

void Menu::openHelp()
{
    juce::DialogWindow::LaunchOptions launchOptions;

    const std::string helpContent =
        "How to Use This App\n"
        "=========================================\n\n"
        "Getting Started:\n"
        "1. Open an existing project via Ctrl + o or from menu File -> Open project\n"
        "   or save a fresh project one via Ctrl + Shift + s or from menu File -> Save project as\n"
        "2. Load an audio file via Ctrl + Shift + a or from menu File -> Add audio file\n"
        "3. Save current project via Ctrl + s or from menu File -> Save project\n"
        "4. Open new project via Ctrl + n or from menu File -> New project\n\n"
        "Basic Controls:\n"
        "• Play/Pause: Space bar or click the play/pause button in the toolbar\n"
        "• Stop: Backspace or click the stop button in the toolbar\n"
        "• Turn on/off split mode for clips: click the split button in the toolbar\n"
        "• Follow mode for the playhead: Click the follow mode button in the toolbar\n"
        "• Track currently played second via the counter in the top right corner\n"
        "• Track the time of splitting an audio clip when split mode is active in the top right\n\n"
        "Other Features:\n"
        "• Left-click and drag on top of the playhead to change current time\n"
        "• Right-click on a track or a clip to show respective context menu\n"
        "• Left-click and drag near the track name to reorder tracks\n"
        "• Double left-click on a track name to change it\n"
        "• Left-click M/S buttons to mute/solo the track\n"
        "• Change gain, panner and reverb properties via options on the left side of the app\n"
        "• Adjust zoom level of waveforms via Zoom level slider in the left corner of the app\n"
        "• Apply fade in/fade by left-clicking and dragging from beginning/end of the clip\n"
        "• Change the type of function used for fade by right-clicking\n\n";

    auto* textEditor = new juce::TextEditor();
    textEditor->setMultiLine(true);
    textEditor->setReadOnly(true);
    textEditor->setScrollbarsShown(true);
    textEditor->setCaretVisible(false);
    textEditor->setText(helpContent);
    textEditor->applyFontToAllText(juce::Font(16.0f));
    textEditor->setColour(juce::TextEditor::textColourId, juce::Colours::whitesmoke);
    textEditor->setColour(juce::TextEditor::backgroundColourId, juce::Colour(0xff0e345a));
    textEditor->setJustification(juce::Justification::centred);

    constexpr int dialogWidth{650};
    constexpr int dialogHeight{500};

    textEditor->setSize(dialogWidth, dialogHeight);

    launchOptions.content.setOwned(textEditor);
    launchOptions.content->setVisible(true);

    launchOptions.dialogTitle = "Manual";
    launchOptions.dialogBackgroundColour = juce::Colour(0xff0e345a);
    launchOptions.escapeKeyTriggersCloseButton = true;
    launchOptions.useNativeTitleBar = true;
    launchOptions.resizable = true;

    juce::DialogWindow* dialogWindow = launchOptions.launchAsync();
    dialogWindow->centreWithSize(dialogWidth, dialogHeight);
}
