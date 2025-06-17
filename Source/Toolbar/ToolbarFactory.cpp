#include "ToolbarFactory.h"
#include "../Constants.h"

ToolbarFactory::ToolbarFactory(juce::ValueTree& parentTree) : tree(parentTree) { initPlayheadFollowMode(); }

void ToolbarFactory::initPlayheadFollowMode() const
{
    assert(!tree.hasProperty(ValueTreeIDs::followModeChanged));
    tree.setProperty(
        ValueTreeIDs::followModeChanged, static_cast<int>(PlayheadFollowConstants::Mode::NoFollow), nullptr);
}
void ToolbarFactory::getAllToolbarItemIds(juce::Array<int>& ids)
{
    const juce::Array<int> toolbarButtons{separatorBarId,
                                          spacerId,
                                          flexibleSpacerId,
                                          previous,
                                          next,
                                          replay,
                                          playPause,
                                          stop,
                                          startRecording,
                                          clipSplit,
                                          followMode};
    ids.addArray(toolbarButtons);
}

void ToolbarFactory::getDefaultItemSet(juce::Array<int>& ids)
{
// TODO: Delete this shit and fix toolbar length
#define REPEAT7(x) x, x, x, x, x, x, x
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
                                                 stop,
                                                 separatorBarId,
                                                 separatorBarId,
                                                 clipSplit,
                                                 flexibleSpacerId,
                                                 followMode,
                                                 REPEAT7(spacerId)};
    ids.addArray(toolbarDefaultButtons);
}

juce::ToolbarItemComponent* ToolbarFactory::createItem(const int itemId)
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
        case stop:
            stopButton = createButtonFromImage(stop, "Stop");
            stopButton->addListener(this);
            return stopButton;
        case clipSplit:
            clipSplitButton = createButtonFromImage(clipSplit, "Clip split");
            clipSplitButton->addListener(this);
            return clipSplitButton;
        case followMode:
            followModeButton = createButtonFromImage(followMode, "Follow mode");
            followModeButton->addListener(this);
            return followModeButton;
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
        case stop:
            png = BinaryData::stopButton_png;
            pngSize.emplace(BinaryData::stopButton_pngSize);
            break;
        case clipSplit:
            png = BinaryData::split_png;
            pngSize.emplace(BinaryData::split_pngSize);
            break;
        case followMode:
            png = BinaryData::followModeButton_png;
            pngSize.emplace(BinaryData::followModeButton_pngSize);
            break;
        default:
            std::unreachable();
    }

    assert(png.has_value() and pngSize.has_value());

    return new juce::ToolbarButton(
        itemId, "juce!", juce::Drawable::createFromImageData(png.value(), pngSize.value()), {});
}

void ToolbarFactory::temporaryButtonsFunction(const juce::String& buttonName) const
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
    else if(button == nextButton)
    {
        nextButtonClicked();
    }
    else if(button == replayButton)
    {
        replayButtonClicked();
    }
    else if(button == playPauseButton)
    {
        playPauseButtonClicked();
    }
    else if(button == startRecordingButton)
    {
        startRecordingButtonClicked();
    }
    else if(button == stopButton)
    {
        stopButtonClicked();
    }
    else if(button == followModeButton)
    {
        followModeButtonClicked();
    }
    else if(button == clipSplitButton)
    {
        clipSplitButtonClicked();
    }
}

void ToolbarFactory::previousButtonClicked() const { temporaryButtonsFunction("previousButton"); }
void ToolbarFactory::nextButtonClicked() const { temporaryButtonsFunction("nextButton"); }
void ToolbarFactory::replayButtonClicked() const { temporaryButtonsFunction("replayButton"); }
void ToolbarFactory::playPauseButtonClicked() const
{
    tree.setProperty(ValueTreeIDs::playPauseButtonClicked, true, nullptr);
    tree.setProperty(ValueTreeIDs::playPauseButtonClicked, ValueTreeConstants::doNothing, nullptr);
}
void ToolbarFactory::startRecordingButtonClicked() const { temporaryButtonsFunction("startRecordingButton"); }
void ToolbarFactory::stopButtonClicked() const
{
    tree.setProperty(ValueTreeIDs::stopButtonClicked, true, nullptr);
    tree.setProperty(ValueTreeIDs::stopButtonClicked, ValueTreeConstants::doNothing, nullptr);
}

void ToolbarFactory::followModeButtonClicked() const
{
    assert(tree.hasProperty(ValueTreeIDs::followModeChanged));
    const int currentMode = tree[ValueTreeIDs::followModeChanged];

    juce::PopupMenu menu;
    menu.addItem(1, "No Follow", true, currentMode == static_cast<int>(PlayheadFollowConstants::Mode::NoFollow));
    menu.addItem(
        2, "Smooth Follow", true, currentMode == static_cast<int>(PlayheadFollowConstants::Mode::SmoothFollow));
    menu.addItem(3, "Jump Follow", true, currentMode == static_cast<int>(PlayheadFollowConstants::Mode::JumpFollow));

    menu.showMenuAsync(juce::PopupMenu::Options().withTargetComponent(followModeButton),
                       [this](const int result)
                       {
                           if(result > 0)
                           {
                               tree.setProperty(ValueTreeIDs::followModeChanged, result, nullptr);
                           }
                       });
}

// TURN OFF isClipSplitActive WHEN CLICKING ANYTHING!!!!!
void ToolbarFactory::clipSplitButtonClicked()
{
    isClipSplitActive = not isClipSplitActive;
    tree.setProperty(ValueTreeIDs::toggleSplitAudioClipMode, isClipSplitActive, nullptr);
    if(isClipSplitActive)
    {
        clipSplitButton->setButtonText("Split");
        clipSplitButton->setStyle(juce::Toolbar::ToolbarItemStyle::iconsWithText);
    }
    else
    {
        clipSplitButton->setStyle(juce::Toolbar::ToolbarItemStyle::iconsOnly);
    }
}

