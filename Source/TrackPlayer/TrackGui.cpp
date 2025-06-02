#include "TrackGui.h"
#include "TrackGuiManager.h"

TrackGui::TrackGui(const uint16_t boxWidth, const int numOfSeconds, juce::ValueTree& parentTree) :
    tree{parentTree}, currentBoxWidth{boxWidth}, currentNumOfSeconds{numOfSeconds}
{
    // initPopUpMenuForTrack();
    // initPopUpMenuForClip();
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

void TrackGui::mouseDown(const juce::MouseEvent& event)
{
    if(event.mods.isRightButtonDown())
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
    if(event.mods.isLeftButtonDown())
    {
        triggerTrackGuiAction(ValueTreeIDs::setSelectedTrack);
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
