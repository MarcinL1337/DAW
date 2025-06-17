#include "TrackGui.h"
#include "TrackGuiManager.h"

ClipSplitBar TrackGui::clipSplitBar;

TrackGui::TrackGui(const uint16_t boxWidth, const int numOfSeconds, juce::ValueTree& parentTree) :
    tree{parentTree}, currentBoxWidth{boxWidth}, currentNumOfSeconds{numOfSeconds}
{
    tree.addListener(this);
}

void TrackGui::paint(juce::Graphics& g)
{
    g.setColour(juce::Colours::lightgrey);

    for(auto i{0u}; i < currentNumOfSeconds; i++)
    {
        // TODO: take this into consideration: https://forum.juce.com/t/best-practice-for-2d-graph-scrolling/13359/5
        g.drawRect(i * getBoxWidthToFloat(), 0.0, getBoxWidthToFloat(), getBoxHeightToFloat(), 0.75);
    }
}

void TrackGui::resized()
{
    for(const auto& waveform: waveforms) { waveform->setBounds(getLocalBounds()); }
}

void TrackGui::makeNewWaveformFromAudioFilePath(const juce::String& newAudioFilePath, const NodeID newAudioClipID)
{
    auto waveform = std::make_unique<Waveform>(newAudioFilePath, currentBoxWidth, tree, newAudioClipID);
    waveforms.emplace_back(std::move(waveform));
    addAndMakeVisible(waveforms.back().get());
    resized();
}

void TrackGui::addNewAudioFile(const juce::String& newAudioFilePath, const NodeID newAudioClipID)
{
    makeNewWaveformFromAudioFilePath(newAudioFilePath, newAudioClipID);
}

void TrackGui::handleLeftMouseClick(const juce::MouseEvent& event)
{
    if(isClipSplitActive)
    {
        handleClipSplit(event);
    }
    triggerTrackGuiAction(ValueTreeIDs::setSelectedTrack);
}

void TrackGui::handleRightMouseClick(const juce::MouseEvent& event)
{
    for(const auto& waveform: waveforms)
    {
        if(waveform->getBoundsInParent().contains(event.mouseDownPosition.x, event.mouseDownPosition.y))
        {
            showPopUpMenuForClip(*waveform);
            return;
        }
    }
    showPopUpMenuForTrack(event.mouseDownPosition.x);
}

// TODO: track delete bug
void TrackGui::mouseDown(const juce::MouseEvent& event)
{
    if(event.mods.isRightButtonDown())
    {
        handleRightMouseClick(event);
    }
    if(event.mods.isLeftButtonDown())
    {
        handleLeftMouseClick(event);
    }
}

void TrackGui::mouseEnter(const juce::MouseEvent& event)
{
    if(isClipSplitActive)
    {
        addAndMakeVisible(&clipSplitBar);
        clipSplitBar.setBounds(getLocalBounds());
    }
}

void TrackGui::mouseExit(const juce::MouseEvent& event)
{
    if(isClipSplitActive)
    {
        removeChildComponent(&clipSplitBar);
    }
}

void TrackGui::mouseMove(const juce::MouseEvent& event)
{
    if(isClipSplitActive)
    {
        const auto eventMouseX{event.mouseDownPosition.getX()};
        clipSplitBar.setMouseXPosition(eventMouseX);
        clipSplitBar.repaint();

        const float newSplitSeconds{eventMouseX / currentBoxWidth};
        tree.setProperty(ValueTreeIDs::splitSecondsChanged, newSplitSeconds, nullptr);
    }
}

juce::PopupMenu TrackGui::initPopUpMenuForTrack()
{
    juce::PopupMenu trackMenu;

    trackMenu.addItem(deleteTrack, "Delete track");
    trackMenu.addItem(duplicateTrack, "Duplicate the track");
    trackMenu.addItem(pasteAudioClip, "Paste audio clip", isAnyWaveformCopied, false);

    return trackMenu;
}

void TrackGui::showPopUpMenuForTrack(const float clickOffset)
{
    auto trackMenu = initPopUpMenuForTrack();
    trackMenu.showMenuAsync(juce::PopupMenu::Options(),
                            [this, clickOffset](const int option)
                            {
                                switch(option)
                                {
                                    case noOptionChosen:
                                        break;
                                    case deleteTrack:
                                        triggerTrackGuiAction(ValueTreeIDs::deleteTrackGui);
                                        break;
                                    case duplicateTrack:
                                        triggerTrackGuiAction(ValueTreeIDs::duplicateTrackGui);
                                        break;
                                    case pasteAudioClip:
                                        handleClipPaste(clickOffset);
                                        break;
                                    default:
                                        std::unreachable();
                                }
                            });
}

juce::PopupMenu TrackGui::initPopUpMenuForClip()
{
    juce::PopupMenu clipMenu;

    clipMenu.addItem(deleteAudioClip, "Delete audio clip");
    clipMenu.addItem(copyAudioClip, "Copy audio clip");
    clipMenu.addItem(cutAudioClip, "Cut audio clip");

    return clipMenu;
}

void TrackGui::showPopUpMenuForClip(const Waveform& clipWaveform)
{
    auto clipMenu = initPopUpMenuForClip();
    clipMenu.showMenuAsync(juce::PopupMenu::Options(),
                           [this, &clipWaveform](const int option)
                           {
                               switch(option)
                               {
                                   case noOptionChosen:
                                       break;
                                   case deleteAudioClip:
                                       handleClipDelete(clipWaveform);
                                       break;
                                   case copyAudioClip:
                                       handleClipCopy(clipWaveform);
                                       break;
                                   case cutAudioClip:
                                       handleClipCut(clipWaveform);
                                       break;
                                   default:
                                       std::unreachable();
                               }
                           });
}

void TrackGui::setOffsetOfWaveformInSeconds(const NodeID audioClipID, const double offsetSeconds) const
{
    for(const auto& waveform: waveforms)
        if(waveform->getAudioClipID() == audioClipID)
        {
            waveform->setOffsetSeconds(offsetSeconds);
            break;
        }
}

void TrackGui::changeBoxWidth(const uint16_t newBoxWidth)
{
    currentBoxWidth = newBoxWidth;
    for(const auto& waveform: waveforms) { waveform->changeBoxWidth(newBoxWidth); }
}

void TrackGui::changeBoxHeight(const uint16_t newBoxHeight)
{
    currentBoxHeight = newBoxHeight;
    for(const auto& waveform: waveforms) { waveform->changeBoxHeight(newBoxHeight); }
}

void TrackGui::triggerTrackGuiAction(const juce::Identifier& actionId) const
{
    const auto trackPlayer = findParentComponentOfClass<TrackGuiManager>();
    const auto currentTrackIndex = std::distance(
        trackPlayer->trackGuiVector.begin(),
        std::ranges::find_if(trackPlayer->trackGuiVector, [this](auto& trackGui) { return this == trackGui.get(); }));

    tree.setProperty(actionId, currentTrackIndex, nullptr);
    tree.setProperty(actionId, ValueTreeConstants::doNothing, nullptr);
}

void TrackGui::handleClipDelete(const Waveform& clipWaveform)
{
    const auto trackPlayer = findParentComponentOfClass<TrackGuiManager>();
    const auto currentTrackIndex = std::distance(
        trackPlayer->trackGuiVector.begin(),
        std::ranges::find_if(trackPlayer->trackGuiVector, [this](auto& trackGui) { return this == trackGui.get(); }));

    const juce::Array<juce::var> deletedClipInfo{currentTrackIndex,
                                                 static_cast<int>(clipWaveform.getAudioClipID().uid)};
    tree.setProperty(ValueTreeIDs::deleteAudioClip, deletedClipInfo, nullptr);

    waveforms.erase(std::ranges::find_if(
        waveforms, [&clipWaveform](auto& waveformPtr) { return waveformPtr.get() == &clipWaveform; }));
}

void TrackGui::handleClipCopy(const Waveform& clipWaveform)
{
    isAnyWaveformCopied = true;
    tree.setProperty(ValueTreeIDs::copyAudioClip, static_cast<int>(clipWaveform.getAudioClipID().uid), nullptr);
    tree.setProperty(ValueTreeIDs::copyAudioClip, ValueTreeConstants::doNothing, nullptr);
}

void TrackGui::handleClipCut(const Waveform& clipWaveform)
{
    handleClipCopy(clipWaveform);
    handleClipDelete(clipWaveform);
}

void TrackGui::handleClipPaste(const float clickOffset)
{
    const auto trackPlayer = findParentComponentOfClass<TrackGuiManager>();
    const int currentTrackIndex = static_cast<int>(std::distance(
        trackPlayer->trackGuiVector.begin(),
        std::ranges::find_if(trackPlayer->trackGuiVector, [this](auto& trackGui) { return this == trackGui.get(); })));

    float clickPositionToSeconds{clickOffset / static_cast<float>(currentBoxWidth)};
    const juce::Array<juce::var> pasteInfo{currentTrackIndex, clickPositionToSeconds};

    tree.setProperty(ValueTreeIDs::pasteAudioClip, pasteInfo, nullptr);
    isAnyWaveformCopied = false;
}

void TrackGui::handleClipSplit(const juce::MouseEvent& event)
{
    const auto trackPlayer = findParentComponentOfClass<TrackGuiManager>();
    const auto currentTrackIndex = std::distance(
        trackPlayer->trackGuiVector.begin(),
        std::ranges::find_if(trackPlayer->trackGuiVector, [this](auto& trackGui) { return this == trackGui.get(); }));

    for(const auto& waveform: waveforms)
    {
        if(waveform->getBoundsInParent().contains(event.mouseDownPosition.x, event.mouseDownPosition.y))
        {
            const auto waveformWidth = waveform->getBounds().getWidth();
            const auto waveformOffset = waveform->getBounds().getX();
            const float waveformSplit =
                (event.mouseDownPosition.x - static_cast<float>(waveformOffset)) / static_cast<float>(waveformWidth);

            const juce::Array<juce::var> splitClipInfo{
                currentTrackIndex, static_cast<int>(waveform->getAudioClipID().uid), waveformSplit};

            waveforms.erase(std::ranges::find_if(
                waveforms, [&waveform](auto& waveformPtr) { return waveformPtr.get() == waveform.get(); }));
            tree.setProperty(ValueTreeIDs::splitAudioClip, splitClipInfo, nullptr);

            return;
        }
    }
}

void TrackGui::valueTreePropertyChanged(juce::ValueTree&, const juce::Identifier& property)
{
    if(static_cast<int>(tree[property]) == ValueTreeConstants::doNothing)
        return;
    if(property == ValueTreeIDs::toggleSplitAudioClipMode)
    {
        isClipSplitActive = tree[ValueTreeIDs::toggleSplitAudioClipMode];
    }
}
