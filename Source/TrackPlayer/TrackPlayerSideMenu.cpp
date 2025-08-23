#include "TrackPlayerSideMenu.h"
#include "../TrackManager.h"

TrackPlayerSideMenu::TrackPlayerSideMenu(juce::ValueTree& parentTree) : tree{parentTree}
{
    tree.addListener(this);
    addMouseListener(this, true);
}

void TrackPlayerSideMenu::paint(juce::Graphics& g)
{
    if(currentSelectedTrack != TrackPlayerConstants::noTrackChosen)
    {
        g.setColour(juce::Colours::darkgrey);
        g.fillRect(0, currentSelectedTrack * currentTrackGuiBoxHeight, getWidth(), currentTrackGuiBoxHeight);
    }

    g.setColour(juce::Colours::whitesmoke);
    for(auto i{0u}; i < getCurrentNumberOfTracks() + 1; ++i)
    {
        g.drawLine(0, i * currentTrackGuiBoxHeight, getWidth(), i * currentTrackGuiBoxHeight, 0.75);
    }
}

void TrackPlayerSideMenu::paintOverChildren(juce::Graphics& g)
{
    if(!isDragging)
        return;

    juce::Rectangle dragBounds = {0,
                                  0,
                                  static_cast<int>(getWidth() * dragScaleFactor),
                                  static_cast<int>(currentTrackGuiBoxHeight * dragScaleFactor)};
    dragBounds.setCentre(currentDragPosition);
    dragBounds = dragBounds.constrainedWithin(getLocalBounds());

    g.setColour(juce::Colours::black.withAlpha(0.65f));
    g.fillRoundedRectangle(dragBounds.toFloat(), 5.0f);
    g.setColour(juce::Colours::white.withAlpha(0.8f));
    g.drawRoundedRectangle(dragBounds.toFloat(), 5.0f, 2.0f);

    g.drawText(
        trackControlsVector[draggedTrackIndex].trackNameLabel->getText(), dragBounds, juce::Justification::centred);
}

void TrackPlayerSideMenu::resized()
{
    trackButtonsSize = static_cast<uint16_t>(0.15 * getWidth());
    buttonMargin = static_cast<uint16_t>(0.01 * getWidth());
}

void TrackPlayerSideMenu::valueTreePropertyChanged(juce::ValueTree&, const juce::Identifier& property)
{
    if(static_cast<int>(tree[property]) == ValueTreeConstants::doNothing)
        return;

    if(property == ValueTreeIDs::setSelectedTrack)
    {
        currentSelectedTrack = tree[ValueTreeIDs::setSelectedTrack];
        repaint();
    }
    else if(property == ValueTreeIDs::reorderTracks)
    {
        const int fromIndex = tree[ValueTreeIDs::reorderTracks][0];
        const int toIndex = tree[ValueTreeIDs::reorderTracks][1];

        auto trackControlToMove = std::move(trackControlsVector[fromIndex]);
        trackControlsVector.erase(trackControlsVector.begin() + fromIndex);
        trackControlsVector.insert(trackControlsVector.begin() + toIndex, std::move(trackControlToMove));

        for(int i = 0; i < trackControlsVector.size(); ++i)
        {
            auto currentTrackButtonsArea = getCurrentTrackButtonsArea(i);
            auto currentTrackNameArea = getCurrentTrackNameArea(i);

            setupSoloButton(trackControlsVector[i].soloButton, currentTrackButtonsArea, i);
            setupMuteButton(trackControlsVector[i].muteButton, currentTrackButtonsArea, i);
            setupTrackNameLabel(trackControlsVector[i].trackNameLabel, currentTrackNameArea, i);
        }

        repaint();
    }
}

void TrackPlayerSideMenu::mouseDown(const juce::MouseEvent& event)
{
    if(not event.mods.isLeftButtonDown())
        return;

    draggedTrackIndex = getTrackIndexFromMousePosition(event.getEventRelativeTo(this).getPosition());
    tree.setProperty(ValueTreeIDs::setSelectedTrack, draggedTrackIndex, nullptr);
    if(draggedTrackIndex != TrackPlayerConstants::noTrackChosen)
        trackControlsVector[draggedTrackIndex].setAlpha(0.7f);
}

void TrackPlayerSideMenu::mouseDrag(const juce::MouseEvent& event)
{
    if(draggedTrackIndex != TrackPlayerConstants::noTrackChosen && event.mouseWasDraggedSinceMouseDown())
    {
        if(not isDragging)
        {
            isDragging = true;
            trackControlsVector[draggedTrackIndex].setAlpha(0.3f);
        }
        currentDragPosition = event.getEventRelativeTo(this).getPosition();
        dropTargetTrackIndex = getTrackIndexFromMousePosition(currentDragPosition);

        if(dropTargetTrackIndex != TrackPlayerConstants::noTrackChosen && dropTargetTrackIndex != draggedTrackIndex)
        {
            const juce::Array<juce::var> reorderInfo{draggedTrackIndex, dropTargetTrackIndex};
            tree.setProperty(ValueTreeIDs::reorderTracks, reorderInfo, nullptr);
            tree.setProperty(ValueTreeIDs::reorderTracks, ValueTreeConstants::doNothing, nullptr);
            tree.setProperty(ValueTreeIDs::setSelectedTrack, dropTargetTrackIndex, nullptr);

            draggedTrackIndex = dropTargetTrackIndex;
        }

        repaint();
    }
}

void TrackPlayerSideMenu::mouseUp(const juce::MouseEvent& event)
{
    if(draggedTrackIndex == TrackPlayerConstants::noTrackChosen)
        return;

    trackControlsVector[draggedTrackIndex].setAlpha(1.0f);

    draggedTrackIndex = TrackPlayerConstants::noTrackChosen;
    dropTargetTrackIndex = TrackPlayerConstants::noTrackChosen;
    isDragging = false;

    repaint();
}

int TrackPlayerSideMenu::getTrackIndexFromMousePosition(const juce::Point<int> position) const
{
    const int trackIdx = (position.y >= 0 ? position.y : 0) / currentTrackGuiBoxHeight;
    return trackIdx < getCurrentNumberOfTracks() ? trackIdx : TrackPlayerConstants::noTrackChosen;
}

void TrackPlayerSideMenu::resizeAllTrackButtons(const int newBoxHeight)
{
    currentTrackGuiBoxHeight = newBoxHeight;

    int currentRow{0};
    for(auto& [soloButton, muteButton, _]: trackControlsVector)
    {
        juce::Rectangle<int> currentTrackButtonsArea{
            getWidth() / 2, currentRow * currentTrackGuiBoxHeight, getWidth() / 2, currentTrackGuiBoxHeight};

        soloButton->setBounds(currentTrackButtonsArea.removeFromRight(trackButtonsSize)
                                  .withSizeKeepingCentre(trackButtonsSize, trackButtonsSize)
                                  .reduced(buttonMargin));

        muteButton->setBounds(currentTrackButtonsArea.removeFromRight(trackButtonsSize)
                                  .withSizeKeepingCentre(trackButtonsSize, trackButtonsSize)
                                  .reduced(buttonMargin));

        currentRow++;
    }
}

void TrackPlayerSideMenu::addTrackControls()
{
    const int currentRow{getCurrentNumberOfTracks()};
    auto currentTrackButtonsArea = getCurrentTrackButtonsArea(currentRow);
    auto currentTrackNameArea = getCurrentTrackNameArea(currentRow);

    auto soloButton = std::make_unique<juce::TextButton>("S");
    auto muteButton = std::make_unique<juce::TextButton>("M");
    auto trackNameLabel = std::make_unique<juce::Label>("", "Track nr " + std::to_string(currentRow + 1));

    setupSoloButton(soloButton, currentTrackButtonsArea, currentRow);
    setupMuteButton(muteButton, currentTrackButtonsArea, currentRow);
    setupTrackNameLabel(trackNameLabel, currentTrackNameArea, currentRow);

    trackControlsVector.push_back({std::move(soloButton), std::move(muteButton), std::move(trackNameLabel)});

    addAndMakeVisible(trackControlsVector.back().soloButton.get());
    addAndMakeVisible(trackControlsVector.back().muteButton.get());
    addAndMakeVisible(trackControlsVector.back().trackNameLabel.get());

    incrementCurrentNumberOfTracks();
}

void TrackPlayerSideMenu::setupSoloButton(const std::unique_ptr<juce::TextButton>& soloButton,
                                          juce::Rectangle<int>& buttonArea, const uint16_t currentRow) const
{
    soloButton->setBounds(buttonArea.removeFromRight(trackButtonsSize)
                              .withSizeKeepingCentre(trackButtonsSize, trackButtonsSize)
                              .reduced(buttonMargin));
    soloButton->onClick = [this, currentRow, soloButtonPtr = soloButton.get()]()
    {
        tree.setProperty(ValueTreeIDs::soloButtonClicked, ValueTreeConstants::doNothing, nullptr);
        tree.setProperty(ValueTreeIDs::soloButtonClicked, currentRow, nullptr);
        tree.setProperty(ValueTreeIDs::setSelectedTrack, currentRow, nullptr);
        const bool currentState = soloButtonPtr->getToggleState();
        soloButtonPtr->setToggleState(!currentState, juce::dontSendNotification);
    };
}

void TrackPlayerSideMenu::setupMuteButton(const std::unique_ptr<juce::TextButton>& muteButton,
                                          juce::Rectangle<int>& buttonArea, const uint16_t currentRow) const
{
    muteButton->setBounds(buttonArea.removeFromRight(trackButtonsSize)
                              .withSizeKeepingCentre(trackButtonsSize, trackButtonsSize)
                              .reduced(buttonMargin));
    muteButton->onClick = [this, currentRow, muteButtonPtr = muteButton.get()]()
    {
        tree.setProperty(ValueTreeIDs::muteButtonClicked, ValueTreeConstants::doNothing, nullptr);
        tree.setProperty(ValueTreeIDs::muteButtonClicked, currentRow, nullptr);
        tree.setProperty(ValueTreeIDs::setSelectedTrack, currentRow, nullptr);
        const bool currentState = muteButtonPtr->getToggleState();
        muteButtonPtr->setToggleState(!currentState, juce::dontSendNotification);
    };
}

void TrackPlayerSideMenu::setupTrackNameLabel(const std::unique_ptr<juce::Label>& trackNameLabel,
                                              juce::Rectangle<int>& trackNameArea, const uint16_t currentRow)
{
    trackNameLabel->setBounds(trackNameArea.removeFromRight(0.9 * trackNameArea.getWidth()));
    trackNameLabel->setEditable(false, true, false);

    trackNameLabel->onTextChange = [this, currentRow, labelPtr = trackNameLabel.get()]()
    {
        const juce::Array<juce::var> nameInfo{currentRow, labelPtr->getText()};
        tree.setProperty(ValueTreeIDs::trackNameChanged, nameInfo, nullptr);
        tree.setProperty(ValueTreeIDs::trackNameChanged, ValueTreeConstants::doNothing, nullptr);
    };
}

juce::Rectangle<int> TrackPlayerSideMenu::getCurrentTrackButtonsArea(const uint16_t currentRow) const
{
    return juce::Rectangle<int>{
        getWidth() / 2, currentRow * currentTrackGuiBoxHeight, getWidth() / 2, currentTrackGuiBoxHeight};
}

juce::Rectangle<int> TrackPlayerSideMenu::getCurrentTrackNameArea(const uint16_t currentRow) const
{
    return juce::Rectangle<int>{0,
                                currentRow * currentTrackGuiBoxHeight + currentTrackGuiBoxHeight / 4,
                                getWidth() / 2,
                                currentTrackGuiBoxHeight / 2};
}

void TrackPlayerSideMenu::removeTrackControls(int trackIndex)
{
    trackControlsVector.erase(trackControlsVector.begin() + trackIndex);
    resizeAllTrackButtons(currentTrackGuiBoxHeight);
    for(; trackIndex < trackControlsVector.size(); trackIndex++)
    {
        auto& [soloButton, muteButton, trackNameLabel] = trackControlsVector[trackIndex];
        auto currentTrackButtonsArea = getCurrentTrackButtonsArea(trackIndex);
        auto currentTrackNameArea = getCurrentTrackNameArea(trackIndex);
        setupSoloButton(soloButton, currentTrackButtonsArea, trackIndex);
        setupMuteButton(muteButton, currentTrackButtonsArea, trackIndex);
        setupTrackNameLabel(trackNameLabel, currentTrackNameArea, trackIndex);
    }
    decrementCurrentNumberOfTracks();
}

void TrackPlayerSideMenu::updateTrackButtonStates(const int trackIndex, const bool muted, const bool soloed) const
{
    assert(trackIndex >= 0 && trackIndex < trackControlsVector.size());
    trackControlsVector[trackIndex].muteButton->setToggleState(muted, juce::dontSendNotification);
    trackControlsVector[trackIndex].soloButton->setToggleState(soloed, juce::dontSendNotification);
}

void TrackPlayerSideMenu::clearAllTracks()
{
    trackControlsVector.clear();
    currentNumberOfTracks = 0;
    tree.setProperty(ValueTreeIDs::setSelectedTrack, TrackPlayerConstants::noTrackChosen, nullptr);
    repaint();
}

void TrackPlayerSideMenu::setTrackName(const int trackIndex, const juce::String& name) const
{
    assert(trackIndex >= 0 && trackIndex < trackControlsVector.size());
    trackControlsVector[trackIndex].trackNameLabel->setText(name, juce::dontSendNotification);
}