#include "ToolbarFactory.h"
#include "../Constants.h"

ToolbarFactory::ToolbarFactory(juce::ValueTree& valueTree) : tree(valueTree)
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
                                          followMode};
    ids.addArray(toolbarButtons);
}

void ToolbarFactory::getDefaultItemSet(juce::Array<int>& ids)
{
    const juce::Array<int> toolbarDefaultButtons{
        previous,       separatorBarId, next,           separatorBarId, replay,           separatorBarId, playPause,
        separatorBarId, startRecording, separatorBarId, stop,           flexibleSpacerId, followMode,     spacerId,
        spacerId,       spacerId,       spacerId,       spacerId,       spacerId,         spacerId};
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
        case followMode:
            followModeButton = createButtonFromImage(itemId, "Follow mode");
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
    if(button == stopButton)
    {
        stopButtonClicked();
    }
    if(button == followModeButton)
    {
        followModeButtonClicked();
    }
}

void ToolbarFactory::previousButtonClicked() { temporaryButtonsFunction("previousButton"); }
void ToolbarFactory::nextButtonClicked() { temporaryButtonsFunction("nextButton"); }
void ToolbarFactory::replayButtonClicked() { temporaryButtonsFunction("replayButton"); }
void ToolbarFactory::playPauseButtonClicked() const
{
    tree.setProperty(ValueTreeIDs::playPauseButtonClicked, true, nullptr);
    tree.setProperty(ValueTreeIDs::playPauseButtonClicked, ValueTreeConstants::doNothing, nullptr);
}
void ToolbarFactory::startRecordingButtonClicked() { temporaryButtonsFunction("startRecordingButton"); }
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
    menu.addItem(1, "No Follow", true, currentMode == 0);
    menu.addItem(2, "Smooth Follow", true, currentMode == 1);
    menu.addItem(3, "Jump Follow", true, currentMode == 2);

    menu.showMenuAsync(juce::PopupMenu::Options().withTargetComponent(followModeButton),
                       [this](const int result)
                       {
                           if(result > 0)
                           {
                               tree.setProperty(ValueTreeIDs::followModeChanged, result - 1, nullptr);
                           }
                       });
}
