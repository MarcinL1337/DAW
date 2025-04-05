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

constexpr float timelineHeightRatio{0.08f};
constexpr uint8_t timeBarBoxSize{15u};

constexpr float trackPlayerHeightRatio{1.0f - timelineHeightRatio};

constexpr float trackPlayerSideMenuWidthRatio{0.12f};

}  // namespace TrackPlayerConstants

namespace SideMenuConstants
{
constexpr float sideMenuWidthRatio{0.15f};
}  // namespace SideMenuConstants