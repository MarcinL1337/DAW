#include "MainWindow.h"

MainWindow::MainWindow(const juce::String& name) :
    DocumentWindow(name, juce::Desktop::getInstance().getDefaultLookAndFeel().findColour(backgroundColourId),
                   allButtons)
{
    setUsingNativeTitleBar(true);
    setContentOwned(new MainComponent(), true);

    setResizable(true, true);
    Component::setVisible(true);
}

void MainWindow::closeButtonPressed() { juce::JUCEApplication::getInstance()->systemRequestedQuit(); }

void MainWindow::initialise()
{
    setResizeLimits(1200, 600, 4000, 3000);
    setFullScreen(true);
}