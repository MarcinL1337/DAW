#include "MainWindow.h"

MainWindow::MainWindow (const juce::String& name)
        : DocumentWindow (name,
                          juce::Desktop::getInstance().getDefaultLookAndFeel()
                                                      .findColour (backgroundColourId),
                          allButtons)
{
    setUsingNativeTitleBar (true);
    setContentOwned (new MainComponent(), true);

    setResizable (true, true);
    centreWithSize (getWidth(), getHeight());

    setVisible (true);
}

void MainWindow::closeButtonPressed()
{
    juce::JUCEApplication::getInstance()->systemRequestedQuit();
}

void MainWindow::initialise()
{

}