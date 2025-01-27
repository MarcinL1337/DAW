#pragma once

#include <juce_gui_extra/juce_gui_extra.h>
#include "BinaryData.h"
#include <cassert>


class ToolbarFactory final : public juce::ToolbarItemFactory
{
public:
    ToolbarFactory();
    ~ToolbarFactory() override = default;

private:
    void getAllToolbarItemIds (juce::Array <int>& ids) override;
    void getDefaultItemSet (juce::Array <int>& ids) override;
    juce::ToolbarItemComponent* createItem (int itemId) override;
	juce::ToolbarButton* createButtonFromImage(int itemId, const juce::String& shortDescription);

	enum SpecialItemIds
    {
        separatorBarId      = -1,   /**< The item ID for a vertical (or horizontal) separator bar that
                                         can be placed between sets of items to break them into groups. */
        spacerId            = -2,   /**< The item ID for a fixed-width space that can be placed between
                                         items.*/
        flexibleSpacerId    = -3    /**< The item ID for a gap that pushes outwards against the things on
                                         either side of it, filling any available space. */
    };

	enum ToolbarElementsIds
	{
		previous = 1,
		next,
		replay,
		playPause,
		startRecording,
		stopRecording
	};

	juce::ToolbarButton* previousButton;
};

