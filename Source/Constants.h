#pragma once
#include <juce_gui_extra/juce_gui_extra.h>
#include <cstdint>

namespace TopLevelMenuConstants
{
constexpr float topLevelMenuHeightRatio{0.02f};
}  // namespace TopLevelMenuConstants

namespace MainToolbarConstants
{
constexpr float mainToolbarHeightRatio{0.035f};
}  // namespace MainToolbarConstants

namespace TrackPlayerConstants
{
constexpr uint16_t startBoxWidth{70u};
constexpr uint16_t startBoxHeight{85u};
constexpr int startNumOfBoxes{51};

constexpr float timelineHeightRatio{0.08f};
constexpr uint8_t timeBarBoxSize{15u};

constexpr float trackPlayerSideMenuWidthRatio{0.12f};

constexpr int noTrackChosen{-1};

}  // namespace TrackPlayerConstants

namespace SideMenuConstants
{
constexpr float sideMenuWidthRatio{0.15f};
}  // namespace SideMenuConstants

namespace ValueTreeConstants
{
constexpr int doNothing{0x42abcdef};
}  // namespace ValueTreeConstants

namespace ValueTreeIDs
{
inline static const juce::Identifier timeBarTime{"timeBarTime"};
inline static const juce::Identifier playPauseButtonClicked{"playPauseButtonClicked"};
inline static const juce::Identifier stopButtonClicked{"stopButtonClicked"};
inline static const juce::Identifier isCurrentlyDraggingTimeBar{"isCurrentlyDraggingTimeBar"};
inline static const juce::Identifier setPlayheadPosition{"setPlayheadPosition"};
inline static const juce::Identifier followModeChanged{"followModeChanged"};

inline static const juce::Identifier soloButtonClicked{"soloButtonClicked"};
inline static const juce::Identifier muteButtonClicked{"muteButtonClicked"};
inline static const juce::Identifier newAudioFile{"newAudioFile"};

inline static const juce::Identifier trackGainChanged{"trackGainChanged"};
inline static const juce::Identifier trackPanChanged{"trackPanChanged"};
inline static const juce::Identifier trackReverbChanged{"trackReverbChanged"};
inline static const juce::Identifier trackRoomSizeChanged{"trackRoomSizeChanged"};
inline static const juce::Identifier trackDampChanged{"trackDampChanged"};
inline static const juce::Identifier trackWetLevelChanged{"trackWetLevelChanged"};
inline static const juce::Identifier trackDryLevelChanged{"trackDryLevelChanged"};
inline static const juce::Identifier trackWidthChanged{"trackWidthChanged"};
inline static const juce::Identifier trackFreezeChanged{"trackFreezeChanged"};
inline static const juce::Identifier trackNameChanged{"trackNameChanged"};

inline static const juce::Identifier trackPlayerZoomPercentage{"trackPlayerZoomPercentage"};
inline static const juce::Identifier numOfSecondsChanged{"numOfSecondsChanged"};

inline static const juce::Identifier deleteTrackGui{"deleteTrackGui"};
inline static const juce::Identifier duplicateTrackGui{"duplicateTrackGui"};
inline static const juce::Identifier setSelectedTrack{"setSelectedTrack"};

inline static const juce::Identifier deleteAudioClip{"deleteAudioClip"};
inline static const juce::Identifier copyAudioClip{"copyAudioClip"};
inline static const juce::Identifier pasteAudioClip{"pasteAudioClip"};

inline static const juce::Identifier splitAudioClip{"splitAudioClip"};
inline static const juce::Identifier toggleSplitAudioClipMode{"toggleSplitAudioClipMode"};
inline static const juce::Identifier splitSecondsChanged{"splitSecondsChanged"};

inline static const juce::Identifier audioClipFadeChanged{"audioClipFadeChanged"};

inline static const juce::Identifier reorderTracks{"reorderTracks"};
}  // namespace ValueTreeIDs

namespace PlayheadFollowConstants
{
enum class Mode
{
    NoFollow = 1,
    SmoothFollow,
    JumpFollow
};
}  // namespace PlayheadFollowConstants
