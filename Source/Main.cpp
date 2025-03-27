#include <memory>

#include "MainComponent.h"
#include "MainWindow.h"

class GuiAppApplication final : public juce::JUCEApplication
{
public:
    GuiAppApplication() = default;

    const juce::String getApplicationName() override { return "Nasz superancki kurczę DAW"; }
    const juce::String getApplicationVersion() override { return "1.0.0"; }
    bool moreThanOneInstanceAllowed() override { return true; }

    void initialise(const juce::String& commandLine) override
    {
        ignoreUnused(commandLine);

        mainWindow = std::make_unique<MainWindow>(getApplicationName());
        mainWindow->initialise();
    }

    void shutdown() override { mainWindow = nullptr; }

    void systemRequestedQuit() override { quit(); }

    void anotherInstanceStarted(const juce::String& commandLine) override { ignoreUnused(commandLine); }

private:
    std::unique_ptr<MainWindow> mainWindow;
};

START_JUCE_APPLICATION(GuiAppApplication)
