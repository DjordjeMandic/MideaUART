#pragma once
#include <Arduino.h>
#include "Appliance/ApplianceBase.h"
#include "Appliance/AirConditioner/Capabilities.h"
#include "Appliance/AirConditioner/StatusData.h"
#include "Helpers/Helpers.h"

namespace dudanov {
namespace midea {
namespace ac {

// Air conditioner control command
struct Control {
  Optional<float> targetTemp{};
  Optional<Mode> mode{};
  Optional<Preset> preset{};
  Optional<FanSpeed> fanMode{};
  Optional<SwingMode> swingMode{};
};

class AirConditioner : public ApplianceBase {
 public:
  void m_setup() override;
  void m_onIdle() override { m_getStatus(); }
  void control(const Control &control);
  void setPowerState(bool state);
  bool getPowerState() const { return m_mode != MODE_OFF; }
  void togglePowerState() { this->setPowerState(m_mode == MODE_OFF); }
  float getTargetTemp() const { return m_targetTemp; }
  float getIndoorTemp() const { return m_indoorTemp; }
  float getOutdoorTemp() const { return m_outdoorTemp; }
  float getIndoorHum() const { return m_indoorHumidity; }
  float getPowerUsage() const { return m_powerUsage; }
  Mode getMode() const { return m_mode; }
  SwingMode getSwingMode() const { return m_swingMode; }
  FanSpeed getFanMode() const { return m_fanMode; }
  Preset getPreset() const { return m_preset; }
  const Capabilities &getCapabilities() const { return m_capabilities; }
  void displayToggle() { m_displayToggle(); }

 protected:
  void m_getPowerUsage();
  void m_getCapabilities();
  void m_getStatus();
  void m_setStatus(StatusData status);
  void m_displayToggle();
  ResponseStatus m_readStatus(FrameData data);
  Capabilities m_capabilities{};
  Timer m_powerUsageTimer;
  float m_indoorHumidity{};
  float m_indoorTemp{};
  float m_outdoorTemp{};
  float m_targetTemp{};
  float m_powerUsage{};
  Mode m_mode{MODE_OFF};
  Preset m_preset{PRESET_NONE};
  FanSpeed m_fanMode{FAN_AUTO};
  SwingMode m_swingMode{SWING_OFF};
  Preset m_lastPreset{PRESET_NONE};
  StatusData m_status{};
  bool m_sendControl{};
};

}  // namespace ac
}  // namespace midea
}  // namespace dudanov
