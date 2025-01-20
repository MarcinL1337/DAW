#include "Menu.h"

MenuBar::MenuBar() {

}

juce::StringArray MenuBar::getMenuBarNames() {
    return menuBarNames;
};

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
    }

    return options;
};

void MenuBar::menuItemSelected(int menuID, int index) {};

juce::ApplicationCommandTarget* MenuBar::getNextCommandTarget() {};

void MenuBar::getAllCommands(juce::Array<juce::CommandID>& c) {};

void MenuBar::getCommandInfo(juce::CommandID commandID, juce::ApplicationCommandInfo& result) {};

bool MenuBar::perform(const InvocationInfo& info) {};