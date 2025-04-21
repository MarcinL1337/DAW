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
constexpr uint16_t startBoxWidth{70u};
constexpr uint16_t startBoxHeight{85u};
constexpr int startNumOfBoxes{51};

constexpr float timelineHeightRatio{0.08f};
constexpr uint8_t timeBarBoxSize{15u};

constexpr float trackPlayerHeightRatio{1.0f - timelineHeightRatio};

constexpr float trackPlayerSideMenuWidthRatio{0.12f};

}  // namespace TrackPlayerConstants

namespace SideMenuConstants
{
constexpr float sideMenuWidthRatio{0.15f};
}  // namespace SideMenuConstants