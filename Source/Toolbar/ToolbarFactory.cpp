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
    const juce::Array<int> toolbarButtons{
        separatorBarId, spacerId, flexibleSpacerId, playPause, stop, clipSplit, followMode};
    ids.addArray(toolbarButtons);
}

void ToolbarFactory::getDefaultItemSet(juce::Array<int>& ids)
{
    const juce::Array<int> toolbarDefaultButtons{separatorBarId,
                                                 playPause,
                                                 separatorBarId,
                                                 separatorBarId,
                                                 stop,
                                                 separatorBarId,
                                                 separatorBarId,
                                                 clipSplit,
                                                 separatorBarId,
                                                 separatorBarId,
                                                 followMode,
                                                 separatorBarId};
    ids.addArray(toolbarDefaultButtons);
}

juce::ToolbarItemComponent* ToolbarFactory::createItem(const int itemId)
{
    switch(itemId)
    {
        case playPause:
            playPauseButton = createButtonFromImage(playPause, "Play/Pause");
            playPauseButton->addListener(this);
            return playPauseButton;
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
        case playPause:
            png = BinaryData::playPauseButton_png;
            pngSize.emplace(BinaryData::playPauseButton_pngSize);
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

void ToolbarFactory::buttonClicked(juce::Button* button)
{
    if(button == playPauseButton)
    {
        playPauseButtonClicked();
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

void ToolbarFactory::playPauseButtonClicked() const
{
    tree.setProperty(ValueTreeIDs::playPauseButtonClicked, true, nullptr);
    tree.setProperty(ValueTreeIDs::playPauseButtonClicked, ValueTreeConstants::doNothing, nullptr);
}

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

// TODO: think about handling clicking outside of tracks region when isClipSplitActive == true.
// Eg. Should it be turned off in that case or should other actions be restricted etc.
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

