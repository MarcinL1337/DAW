#include "ToolbarFactory.h"

ToolbarFactory::ToolbarFactory() { }

void ToolbarFactory::getAllToolbarItemIds (juce::Array <int>& ids)
{
	const juce::Array<int> toolbarButtons {
		separatorBarId,
		spacerId,
		flexibleSpacerId,
		previous,
		next,
		replay,
		playPause,
		stopRecording,
		startRecording
	};
	ids.addArray(toolbarButtons);
}

void ToolbarFactory::getDefaultItemSet (juce::Array <int>& ids)
{
	const juce::Array<int> toolbarDefaultButtons {
		previous,
		separatorBarId,
		next,
		separatorBarId,
		replay,
		separatorBarId,
		playPause,
		separatorBarId,
		startRecording,
		separatorBarId,
		stopRecording
	};
	ids.addArray(toolbarDefaultButtons);
}

juce::ToolbarItemComponent* ToolbarFactory::createItem (int itemId)
{
	switch (itemId)
	{
		case previous:
			previousButton = createButtonFromImage(previous, "Play previous");
			previousButton->addListener(this);
			// add button on click
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

juce::ToolbarButton* ToolbarFactory::createButtonFromImage(int itemId, [[maybe_unused]] const juce::String& shortDescription)
{
	std::optional<const char*> png;
	std::optional<const int> pngSize;

	switch (itemId)
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

	return new juce::ToolbarButton (itemId, "juce!", juce::Drawable::createFromImageData(png.value(), pngSize.value()), {});
}

void ToolbarFactory::buttonClicked(juce::Button *button)
{

}

void ToolbarFactory::playTrack()
{
	setCurrentTrackState(TrackPlayingState::playing);
}

void ToolbarFactory::stopTrack()
{
	setCurrentTrackState(TrackPlayingState::stopped);
}