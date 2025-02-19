#pragma once

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
constexpr float startBoxWidth{70.0f};
constexpr float startBoxHeight{85.0f};
constexpr uint8_t startNumOfBoxes{51u};
constexpr uint8_t startNumOfBoxesRows{20u};

constexpr float timelineHeightRatio{0.1f};
constexpr float minTimelineHeightRatio{0.05f};

constexpr float trackPlayerHeightRatio{1.0f - timelineHeightRatio};

constexpr float trackPlayerSideMenuWidthRatio{0.15f};
constexpr float minTrackPlayerSideMenuWidthRatio{0.7f};

}  // namespace TrackPlayerConstants

namespace SideMenuConstants
{
constexpr float sideMenuWidthRatio{0.15f};
}  // namespace SideMenuConstants