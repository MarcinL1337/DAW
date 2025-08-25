#include "MainWindow.h"

MainWindow::MainWindow(const juce::String& name) :
    DocumentWindow(name, juce::Desktop::getInstance().getDefaultLookAndFeel().findColour(backgroundColourId),
                   allButtons)
{
    setUsingNativeTitleBar(true);
    setContentOwned(new MainComponent(), true);
    auto* mainComponent = dynamic_cast<MainComponent*>(getContentComponent());
    mainComponent->tree.addListener(this);
    setResizable(false, false);
    Component::setVisible(true);
}

void MainWindow::closeButtonPressed()
{
    if(projectIsDirty)
    {
        const auto options = juce::MessageBoxOptions()
                                 .withIconType(juce::MessageBoxIconType::QuestionIcon)
                                 .withTitle("Unsaved Changes")
                                 .withMessage("You have unsaved changes. Do you want to save before closing?")
                                 .withButton("Save and Close")
                                 .withButton("Close Without Saving")
                                 .withButton("Cancel");

        juce::AlertWindow::showAsync(
            options,
            [this](const int result)
            {
                if(result == 1)
                {
                    shouldCloseAfterSave = true;
                    if(auto* mainComponent = dynamic_cast<MainComponent*>(getContentComponent()))
                    {
                        mainComponent->tree.setProperty(ValueTreeIDs::saveProject, true, nullptr);
                        mainComponent->tree.setProperty(
                            ValueTreeIDs::saveProject, ValueTreeConstants::doNothing, nullptr);
                    }
                }
                else if(result == 2)
                {
                    juce::JUCEApplication::getInstance()->systemRequestedQuit();
                }
            });
    }
    else
    {
        juce::JUCEApplication::getInstance()->systemRequestedQuit();
    }
}

void MainWindow::initialise() { setFullScreen(true); }

void MainWindow::updateWindowTitle()
{
    juce::String title = "Nasz superancki kurcze DAW";
    if(projectIsDirty)
        title += " *";
    setName(title);
}

void MainWindow::valueTreePropertyChanged(juce::ValueTree&, const juce::Identifier& property)
{
    if(static_cast<int>(dynamic_cast<MainComponent*>(getContentComponent())->tree[property]) ==
       ValueTreeConstants::doNothing)
        return;

    if(property == ValueTreeIDs::projectDirtyStateChanged)
    {
        projectIsDirty =
            dynamic_cast<MainComponent*>(getContentComponent())->tree[ValueTreeIDs::projectDirtyStateChanged];
        updateWindowTitle();
        if(not projectIsDirty && shouldCloseAfterSave)
        {
            juce::JUCEApplication::getInstance()->systemRequestedQuit();
        }
    }
}