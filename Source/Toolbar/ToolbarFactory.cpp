#include "ToolbarFactory.h"

ToolbarFactory::ToolbarFactory(juce::ValueTree& valueTree) : tree(valueTree) {};

void ToolbarFactory::getAllToolbarItemIds(juce::Array<int>& ids)
{
    const juce::Array<int> toolbarButtons{
        separatorBarId, spacerId, flexibleSpacerId, previous, next, replay, playPause, stopRecording, startRecording};
    ids.addArray(toolbarButtons);
}

void ToolbarFactory::getDefaultItemSet(juce::Array<int>& ids)
{
    const juce::Array<int> toolbarDefaultButtons{previous,
                                                 separatorBarId,
                                                 next,
                                                 separatorBarId,
                                                 replay,
                                                 separatorBarId,
                                                 playPause,
                                                 separatorBarId,
                                                 startRecording,
                                                 separatorBarId,
                                                 stopRecording};
    ids.addArray(toolbarDefaultButtons);
}

juce::ToolbarItemComponent* ToolbarFactory::createItem(int itemId)
{
    switch(itemId)
    {
        case previous:
            previousButton = createButtonFromImage(previous, "Play previous");
            previousButton->addListener(this);
            return previousButton;
        case next:
            nextButton = createButtonFromImage(next, "Next");
            nextButton->addListener(this);
            return nextButton;
        case replay:
            replayButton = createButtonFromImage(replay, "Replay");
            replayButton->addListener(this);
            return replayButton;
        case playPause:
            playPauseButton = createButtonFromImage(playPause, "Play/Pause");
            playPauseButton->addListener(this);
            return playPauseButton;
        case startRecording:
            startRecordingButton = createButtonFromImage(startRecording, "Start recording");
            startRecordingButton->addListener(this);
            return startRecordingButton;
        case stopRecording:
            stopRecordingButton = createButtonFromImage(stopRecording, "Stop recording");
            stopRecordingButton->addListener(this);
            return stopRecordingButton;
        default:
            std::unreachable();
    }
}

juce::ToolbarButton* ToolbarFactory::createButtonFromImage(int itemId,
                                                           [[maybe_unused]] const juce::String& shortDescription)
{
    std::optional<const char*> png;
    std::optional<const int> pngSize;

    switch(itemId)
    {
        case previous:
            png = BinaryData::previousButton_png;
            pngSize.emplace(BinaryData::previousButton_pngSize);
            break;
        case next:
            png = BinaryData::nextButton_png;
            pngSize.emplace(BinaryData::nextButton_pngSize);
            break;
        case replay:
            png = BinaryData::replayButton_png;
            pngSize.emplace(BinaryData::replayButton_pngSize);
            break;
        case playPause:
            png = BinaryData::playPauseButton_png;
            pngSize.emplace(BinaryData::playPauseButton_pngSize);
            break;
        case startRecording:
            png = BinaryData::recordButton_png;
            pngSize.emplace(BinaryData::recordButton_pngSize);
            break;
        case stopRecording:
            png = BinaryData::stopButton_png;
            pngSize.emplace(BinaryData::stopButton_pngSize);
            break;
        default:
            std::unreachable();
    }

    assert(png.has_value() and pngSize.has_value());

    return new juce::ToolbarButton(
        itemId, "juce!", juce::Drawable::createFromImageData(png.value(), pngSize.value()), {});
}

void ToolbarFactory::temporaryButtonsFunction(const juce::String buttonName)
{
    juce::DialogWindow* dialogWindow;
    juce::DialogWindow::LaunchOptions launchOptions;

    auto* label = new juce::Label();
    label->setText(buttonName + " clicked", juce::dontSendNotification);
    label->setColour(juce::Label::textColourId, juce::Colours::whitesmoke);
    label->setJustificationType(juce::Justification::centred);
    launchOptions.content.setOwned(label);
    launchOptions.content->setVisible(true);

    launchOptions.dialogTitle = "Dialogowe okno!";
    launchOptions.dialogBackgroundColour = juce::Colour(0xff0e345a);
    launchOptions.escapeKeyTriggersCloseButton = true;
    launchOptions.useNativeTitleBar = false;
    launchOptions.resizable = true;

    dialogWindow = launchOptions.launchAsync();
    dialogWindow->centreWithSize(300, 300);
}

void ToolbarFactory::buttonClicked(juce::Button* button)
{
    if(button == previousButton)
    {
        previousButtonClicked();
    }
    if(button == nextButton)
    {
        nextButtonClicked();
    }
    if(button == replayButton)
    {
        replayButtonClicked();
    }
    if(button == playPauseButton)
    {
        playPauseButtonClicked();
    }
    if(button == startRecordingButton)
    {
        startRecordingButtonClicked();
    }
    if(button == stopRecordingButton)
    {
        stopButtonClicked();
    }
}

void ToolbarFactory::previousButtonClicked() { temporaryButtonsFunction("previousButton"); }
void ToolbarFactory::nextButtonClicked() { temporaryButtonsFunction("nextButton"); }
void ToolbarFactory::replayButtonClicked() { temporaryButtonsFunction("replayButton"); }
void ToolbarFactory::playPauseButtonClicked() const
{
    tree.setProperty("playPauseButtonClicked", true, nullptr);
    tree.setProperty("playPauseButtonClicked", ValueTreeConstants::doNothing, nullptr);
}
void ToolbarFactory::startRecordingButtonClicked() { temporaryButtonsFunction("startRecordingButton"); }
void ToolbarFactory::stopButtonClicked() const
{
    tree.setProperty("stopButtonClicked", true, nullptr);
    tree.setProperty("stopButtonClicked", ValueTreeConstants::doNothing, nullptr);
}
