#include "Toolbar.h"

Toolbar::Toolbar()
{

}

Toolbar::~Toolbar()
{

}

void Toolbar::getAllToolbarItemIds (juce::Array <int>& ids)
{
	const juce::Array<int> toolbarButtons {
		separatorBarId,
		spacerId,
		flexibleSpacerId,
		previous,
		next,
		restart,
		playPause,
		stopRecording,
		startRecording
	};
	ids.addArray(toolbarButtons);
}

void Toolbar::getDefaultItemSet (juce::Array <int>& ids)
{
	const juce::Array<int> toolbarDefaultButtons {
		separatorBarId,
		spacerId,
		flexibleSpacerId,
		previous,
		next,
		restart,
		playPause,
		stopRecording,
		startRecording
	};
	ids.addArray(toolbarDefaultButtons);
}

juce::ToolbarItemComponent* Toolbar::createItem (int itemId)
{
	switch (itemId)
	{
		case separatorBarId:
			break;
		case spacerId:
			break;
		case flexibleSpacerId:
			break;
		case previous:
			break;
		case next:
			break;
		case restart:
			break;
		case playPause:
			break;
		case stopRecording:
			break;
		case startRecording:
			break;
		default:
			std::unreachable();
	}
	return nullptr;
}