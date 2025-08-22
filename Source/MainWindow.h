#pragma once

#include "MainComponent.h"

class MainWindow final : public juce::DocumentWindow,
                         public juce::ValueTree::Listener
{
public:
    explicit MainWindow(const juce::String& name);

    void closeButtonPressed() override;
    void initialise();

private:
    void updateWindowTitle();
    void valueTreePropertyChanged(juce::ValueTree& treeWhosePropertyHasChanged,
                                  const juce::Identifier& property) override;

    bool projectIsDirty{false};
    bool shouldCloseAfterSave{false};
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MainWindow)
};