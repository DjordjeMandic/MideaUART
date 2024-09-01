#include "Appliance/AirConditioner/DeviceStatus.h"
#include "Appliance/AirConditioner/StatusData.h"

namespace dudanov {
namespace midea {
namespace ac {

static float s_get_temperature(int integer, int decimal, bool fahrenheits) {
  integer -= 50;

  if (!fahrenheits && decimal > 0)
    return static_cast<float>(integer / 2) + decimal * ((integer >= 0) ? 0.1f : -0.1f);

  if (decimal >= 5)
    return static_cast<float>(integer / 2) + ((integer >= 0) ? 0.5f : -0.5f);

  return static_cast<float>(integer) * 0.5f;
}

bool FrameStatusData::hasStatusData(const FrameData &data) {
  return data.hasID(0xC0) || data.hasID(0xA0) || data.hasID(0xA1);
}

void FrameStatusData::updateStatus(DeviceStatus &s) {
  if (hasID(0xC0))
    return m_statusC0(s);

  if (hasID(0xA0))
    return m_statusA0(s);

  if (hasID(0xA1))
    return m_statusA1(s);
}

void FrameStatusData::m_statusC0(DeviceStatus &s) const {
  // Byte #1
  s.powerStatus = m_getBit(1, 0);
  s.imodeResume = m_getBit(1, 2);
  s.timerMode = m_getBit(1, 4);
  s.test2 = m_getBit(1, 5);
  s.errMark = m_getBit(1, 7);

  // Byte #2
  s.setTemperature = m_getValue(2, 15) + 16;
  s.setTemperature_dot = m_getBit(2, 4);
  s.mode = m_getValue(2, 7, 5);

  // Byte #3
  s.fanSpeed = m_getValue(3, 127);

  if (s.fanSpeed == FAN_FIXED)
    s.fanSpeed = FAN_AUTO;

  // Bytes #4,6
  s.timer_on = 0;

  if (m_getBit(4, 7))
    s.timer_on = m_getValue(4, 127) * 15 + m_getValue(6, 15, 4);

  // Bytes #5,6
  s.timer_off = 0;

  if (m_getBit(5, 7))
    s.timer_off = m_getValue(5, 127) * 15 + m_getValue(6, 15);

  // Byte #7
  s.swing = static_cast<SwingMode>(m_getValue(7, 15));

  // Byte #8
  s.cosySleep = m_getValue(8, 3);
  s.save = m_getBit(8, 3);
  s.lowFreqFan = m_getBit(8, 4);
  s.turbo = m_getBit(8, 5);
  s.feelOwn = m_getBit(8, 7);

  // Byte #9
  s.childSleepMode = m_getBit(9, 0);
  s.naturalFan = m_getBit(9, 1);
  s.dryClean = m_getBit(9, 2);
  s.ptcAssis = m_getBit(9, 3);
  s.eco = m_getBit(9, 4);
  s.cleanUp = m_getBit(9, 5);
  s.selfFeelOwn = m_getBit(9, 7);

  // Byte #10
  s.sleepFunc = m_getBit(10, 0);

  if (!s.turbo)
    s.turbo = m_getBit(10, 1);

  s.tempUnit = m_getBit(10, 2);
  s.exchangeAir = m_getBit(10, 3);
  s.nightLight = m_getBit(10, 4);
  s.catchCold = m_getBit(10, 5);
  s.peakElec = m_getBit(10, 6);
  s.coolFan = m_getBit(10, 7);

  // Byte #11
  auto indoor_temp = m_getValue(11);

  // Byte #12
  auto outdoor_temp = m_getValue(12);

  // Byte #13
  auto new_temp = m_getValue(13, 31);
  s.dusFull = m_getBit(13, 5);

  if (new_temp)
    s.setTemperature = new_temp + 12;

  // Byte #14
  s.pwmMode = m_getValue(14, 15);
  s.light = m_getValue(14, 7, 4);

  // Byte #15
  s.indoor_temp = s_get_temperature(indoor_temp, m_getValue(15, 15), s.tempUnit);
  s.outdoor_temp = s_get_temperature(outdoor_temp, m_getValue(15, 15, 4), s.tempUnit);

  // Byte #16
  s.errInfo = m_getValue(16);

  // Byte #19
  s.humidity = m_getValue(19, 127);

  // Byte #21
  if (m_data.size() >= 24) {
    s.double_temp = m_getBit(21, 6);
    s.Eight_Hot = m_getBit(21, 7);

  } else {
    s.double_temp = 0;
    s.Eight_Hot = 0;
  }

  // Byte #22
  if (m_data.size() >= 23)
    s.hasNoWindFeel = m_getBit(22, 3);
}

void FrameStatusData::m_statusA0(DeviceStatus &s) const {
  // Byte #1
  s.powerStatus = m_getBit(1, 0);
  s.setTemperature = m_getValue(1, 31, 1) + 12;
  s.setTemperature_dot = m_getBit(1, 6);
  s.errMark = m_getBit(1, 7);

  // Byte #2
  s.mode = m_getValue(2, 7, 5);

  // Byte #3
  s.fanSpeed = m_getValue(3, 127);

  // Bytes #4,6
  s.timer_on = 0;

  if (m_getBit(4, 7))
    s.timer_on = m_getValue(4, 127) * 15 + m_getValue(6, 15, 4);

  // Bytes #5,6
  s.timer_off = 0;

  if (m_getBit(5, 7))
    s.timer_off = m_getValue(5, 127) * 15 + m_getValue(6, 15);

  // Byte #7
  s.swing = static_cast<SwingMode>(m_getValue(7, 15));

  // Byte #8
  s.cosySleep = m_getValue(8, 3);
  s.save = m_getBit(8, 3);
  s.lowFreqFan = m_getBit(8, 4);
  s.turbo = m_getBit(8, 5);
  s.feelOwn = m_getBit(8, 7);

  // Byte #9
  s.exchangeAir = m_getBit(9, 1);
  s.dryClean = m_getBit(9, 2);
  s.ptcAssis = m_getBit(9, 3);
  s.eco = m_getBit(9, 4);
  s.cleanUp = m_getBit(9, 5);
  s.tempUnit = m_getBit(9, 7);

  // Byte #10
  s.sleepFunc = m_getBit(10, 0);

  if (!s.turbo)
    s.turbo = m_getBit(10, 1);

  s.catchCold = m_getBit(10, 3);
  s.nightLight = m_getBit(10, 4);
  s.peakElec = m_getBit(10, 5);
  s.naturalFan = m_getBit(10, 6);

  // Byte #11
  s.pwmMode = m_getValue(11, 15);
  s.light = m_getValue(11, 7, 4);

  // Byte #12
  s.setExpand_dot = m_getBit(12, 0);
  s.setExpand = m_getValue(12, 31) + 12;
  s.double_temp = m_getBit(12, 6);
  s.Eight_Hot = m_getBit(12, 7);

  // Byte #13
  s.humidity = m_getValue(13, 127);

  // Byte #14
  s.hasNoWindFeel = m_getBit(14, 3);
}

void FrameStatusData::m_statusA1(DeviceStatus &s) const {
  s.indoor_temp = static_cast<float>(static_cast<int>(m_getValue(13)) - 50) * 0.5f;
  s.outdoor_temp = static_cast<float>(static_cast<int>(m_getValue(14)) - 50) * 0.5f;
  s.humidity = m_getValue(17, 127);
}

void FrameStatusData::to40Command(const DeviceStatus &s) {
  uint8_t fan_speed = s.fanSpeed;
  bool turbo = s.turbo;
  bool eco = s.eco;

  if (!s.powerStatus || s.mode == MODE_FAN_ONLY) {
    eco = false;
    turbo = false;
  }

  if (s.mode != MODE_DRY && fan_speed == FAN_FIXED)
    fan_speed = FAN_AUTO;

  m_data.reserve(25);
  m_data.assign(24, 0);

  m_data[0] = 0x40;
  m_data[1] = s.beeper * 0x40 + s.test2 * 0x20 + s.timerMode * 0x10 + s.childSleepMode * 0x08 + s.imodeResume * 0x04 +
              0x02 + s.powerStatus;

  bool set_temp_dot = s.setTemperature_dot;
  uint8_t set_temp = s.setTemperature;
  uint8_t set_temp_new = (set_temp - 12) % 32;

  set_temp -= 16;

  if (set_temp < 1 || set_temp > 14)
    set_temp = 1;

  m_data[2] = s.mode * 0x20 + set_temp_dot * 0x10 + set_temp;
  m_data[3] = fan_speed % 128;

  // Setting timers. Initialized off. Therefore, we process only if enabled.

  m_data[4] = 0x7F;

  if (s.timer_on) {
    m_data[4] = 0x80 | s.timer_on / 15;
    m_data[6] = s.timer_on % 15 * 0x10;
  }

  m_data[5] = 0x7F;

  if (s.timer_off) {
    m_data[5] = 0x80 | s.timer_off / 15;
    m_data[6] |= s.timer_off % 15;
  }

  m_data[7] = 0x30 + s.swing;

  m_data[8] = s.feelOwn * 0x80 + s.powerSaver * 0x40 + turbo * 0x20 + s.lowFreqFan * 0x10 + s.save * 0x08 +
              s.alarmSleep * 0x04 + s.cosySleep % 4;

  m_data[9] = eco * 0x80 + s.changeCosySleep * 0x40 + s.cleanUp * 0x20 + s.ptcButton * 0x10 + s.ptcAssis * 0x08 +
              s.dryClean * 0x04 + s.exchangeAir * 0x02 + s.wiseEye;

  m_data[10] = s.cleanFanTime * 0x80 + s.dusFull * 0x40 + s.peakElec * 0x20 + s.nightLight * 0x10 + s.catchCold * 0x08 +
               s.tempUnit * 0x04 + turbo * 0x02 + s.sleepFunc;

  m_data[15] = s.naturalFan * 0x40;

  m_data[18] = set_temp_new;

  m_data[19] = s.humidity;

  m_data[21] = s.Eight_Hot * 0x80 + s.double_temp * 0x40 + s.setExpand % 32 * 0x02 + s.setExpand_dot;

  m_data[23] = m_getID();

  appendCRC();
}

float DeviceStatus::getTargetTemperature() const {
  float val = static_cast<float>(setTemperature);

  if (setTemperature_dot)
    val += 0.5f;

  return val;
}

void DeviceStatus::setTargetTemperature(float temperature) {
  const unsigned value = static_cast<unsigned>(temperature * 2.0f + 0.5f);

  setTemperature = value / 2;
  setTemperature_dot = value % 2;
}

FanSpeed DeviceStatus::getFanMode() const {
  FanSpeed speed = static_cast<FanSpeed>(fanSpeed);

  if (speed == 30) {
    speed = FAN_LOW;
  } else if (speed == 50) {
    speed = FAN_MEDIUM;
  }

  return speed;
}

}  // namespace ac
}  // namespace midea
}  // namespace dudanov
