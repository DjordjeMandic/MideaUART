#pragma once
#include <Arduino.h>
#include "Frame/FrameData.h"
#include "StatusData.h"

namespace dudanov {
namespace midea {
namespace ac {

class DeviceStatus {
 public:
  DeviceStatus() = default;
  DeviceStatus(const DeviceStatus &deviceStatus) = default;

  float getTargetTemperature() const;
  void setTargetTemperature(float value);
  FanSpeed getFanMode() const;
  void setFanSpeed(FanSpeed value) { fanSpeed = value; }
  void setFanSpeed(uint8_t value) { fanSpeed = value; }

  /// Beeper Feedback. Only in Command.
  bool beeper;
  /// Frost Protection mode (8 Degrees Heating).
  bool Eight_Hot;

  /// Wind Off Me. Only in `COOL` and `HEAT`. Turn OFF all Swing.
  bool avoidPeople;
  /// Wind On Me. Only in `COOL` and `HEAT`. Turn ON all Swing.
  bool blowingPeople;
  /// Breeze Away.
  bool noWindOnMe;

  /// Reset Air Filter Maintenance Timer. Only in Command.
  bool cleanFanTime;
  /// Air Filter Maintenance.
  bool dusFull;
  /// ECO mode.
  bool eco;
  /// Error Code. Known: 0x26 - Water Full.
  uint8_t errInfo;
  /// Fan Speed.
  uint8_t fanSpeed{FAN_AUTO};
  // Silky Cool.
  bool hasNoWindFeel;
  /// Set Target Humidity in Smart Dry Mode. Fan Speed must be AUTO.
  uint8_t humidity;
  ///
  uint8_t light;
  /// Mode.
  uint8_t mode{MODE_COOL};
  /// Device Status: On/Off.
  bool powerStatus{true};
  /// Electric Auxiliary Heater
  bool ptcAssis;
  /// 39 Self Cleaning.
  bool selfClean;
  /// Target Temperature. Always in Celsius.
  uint8_t setTemperature{26};
  /// Target Temperature +0.5.
  bool setTemperature_dot;
  /// Sleep Preset.
  bool sleepFunc;
  /// Display Temperature in Fahrenheits.
  bool tempUnit;
  /// Current `On Timer` minutes.
  uint16_t timer_on;
  /// Current `Off Timer` minutes.
  uint16_t timer_off;
  /// Turbo Preset.
  bool turbo;

  /// Swing Mode.
  SwingMode swing;

  /// Indoor Temperature. Only in Status. Always in Celsius.
  float indoor_temp;
  /// Outdoor Temperature. Only in Status. Always in Celsius.
  float outdoor_temp;

  bool alarmSleep;
  bool catchCold;
  bool changeCosySleep;
  bool childSleepMode;
  bool cleanUp;
  bool coolFan;
  uint8_t cosySleep;
  bool double_temp;
  bool dryClean;
  bool errMark;
  bool exchangeAir;
  bool feelOwn;
  bool imodeResume;
  bool lowFreqFan;
  bool naturalFan;
  bool nightLight;
  bool peakElec;
  bool powerSaver;
  bool ptcButton;
  uint8_t pwmMode;
  bool save;
  bool selfFeelOwn;
  uint8_t setExpand;
  bool setExpand_dot;
  bool test2;
  bool timerMode;
  bool wiseEye;
};

struct B1Status {
  uint8_t hWindDirection{};
  uint8_t vWindDirection{};
  uint8_t indoorHumidity{};
  std::vector<uint8_t> mMasterValues{};
  std::vector<uint8_t> mSlaveValues{};
  bool isSelfCleanOn{};
  uint8_t breezeType{1};
  bool isBuzzerOn{true};
  bool isNoWindFeelOn{false};
  bool isSoundOn{false};
  bool isBlowingPeopleOn{false};
  bool isAvoidPeopleOn{false};
  bool isOneKeyNoWindOnMeOn{false};
  bool isCombineBreezeOn{false};
  bool isSmartEyeOn{false};
  bool DehumLight{false};
};

class FrameStatusData : public FrameData {
 public:
  void updateFromA0(DeviceStatus &s);
  void updateFromA1(DeviceStatus &s);
  void updateFromC0(DeviceStatus &s);
  void to40Command(const DeviceStatus &s);
};

}  // namespace ac
}  // namespace midea
}  // namespace dudanov
