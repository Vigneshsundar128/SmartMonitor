#pragma once

namespace Config {
namespace Device {
static constexpr const char* kName = "Power Monitor";
static constexpr const char* kFirmwareVersion = "2.0";
}  // namespace Device

namespace Wifi {
static constexpr const char* kSsid = "YOUR_SSID";
static constexpr const char* kPassword = "YOUR_PASSWORD";
static constexpr unsigned long kRetryIntervalMs = 10000UL;
}  // namespace Wifi

namespace Time {
static constexpr long kUtcOffsetSeconds = 19800;
static constexpr const char* kNtpServer = "pool.ntp.org";
static constexpr unsigned long kInitialRetryIntervalMs = 15000UL;
static constexpr unsigned long kResyncIntervalMs = 300000UL;
}  // namespace Time

namespace Weather {
static constexpr const char* kApiUrl = "YOUR_OPENWEATHERMAP_URL";
static constexpr unsigned long kRefreshIntervalMs = 600000UL;
}  // namespace Weather

namespace Web {
static constexpr uint16_t kPort = 80;
}  // namespace Web
}  // namespace Config
