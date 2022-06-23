#include "Appliance/AirConditioner/StatusData.h"
#include "Appliance/AirConditioner/Capabilities.h"

namespace dudanov {
namespace midea {
namespace ac {

float StatusData::getTargetTemp() const {
  float temp = static_cast<float>(this->m_getValue(2, 15) + 16);
  if (this->m_getValue(2, 16))
    temp += 0.5f;
  return temp;
}

void StatusData::setTargetTemp(float temp) {
  uint8_t tmp = static_cast<uint8_t>(temp * 16.0f) + 4;
  this->m_setValue(2, ((tmp & 8) << 1) | (tmp >> 4), 31);
}

static float i16tof(int16_t in) { return static_cast<float>(in - 50) * 0.5f; }
float StatusData::getIndoorTemp() const { return i16tof(this->m_getValue(11)); }
float StatusData::getOutdoorTemp() const { return i16tof(this->m_getValue(12)); }
float StatusData::getHumiditySetpoint() const { return static_cast<float>(this->m_getValue(19, 127)); }

Mode StatusData::getMode() const { return this->m_getPower() ? this->getRawMode() : Mode::MODE_OFF; }

void StatusData::setMode(Mode mode) {
  if (mode != Mode::MODE_OFF) {
    this->m_setPower(true);
    this->m_setValue(2, mode, 7, 5);
  } else {
    this->m_setPower(false);
  }
}

Preset StatusData::getPreset() const {
  if (this->m_getEco())
    return Preset::PRESET_ECO;
  if (this->m_getTurbo())
    return Preset::PRESET_TURBO;
  if (this->m_getSleep())
    return Preset::PRESET_SLEEP;
  if (this->m_getFreezeProtection())
    return Preset::PRESET_FREEZE_PROTECTION;
  return Preset::PRESET_NONE;
}

void StatusData::setPreset(Preset preset) {
  this->m_setEco(false);
  this->m_setSleep(false);
  this->m_setTurbo(false);
  this->m_setFreezeProtection(false);
  switch (preset) {
    case Preset::PRESET_NONE:
      break;
    case Preset::PRESET_ECO:
      this->m_setEco(true);
      break;
    case Preset::PRESET_TURBO:
      this->m_setTurbo(true);
      break;
    case Preset::PRESET_SLEEP:
      this->m_setSleep(true);
      break;
    case Preset::PRESET_FREEZE_PROTECTION:
      this->m_setFreezeProtection(true);
      break;
    default:
      break;
  }
}

static uint8_t bcd2u8(uint8_t bcd) { return 10 * (bcd >> 4) + (bcd & 15); }

float StatusData::getPowerUsage() const {
  uint32_t power = 0;
  const uint8_t *ptr = this->m_data.data() + 18;
  for (uint32_t weight = 1;; weight *= 100, --ptr) {
    power += weight * bcd2u8(*ptr);
    if (weight == 10000)
      return static_cast<float>(power) * 0.1f;
  }
}

B1QueryData::B1QueryData(const CmdB5 &cmdB5) : DataBody({177, 0}) {
  if (cmdB5.hasBlowingPeople) {
    this->m_data.push_back(50);
    this->m_data.push_back(0);
  }
  if (cmdB5.hasAvoidPeople) {
    this->m_data.push_back(51);
    this->m_data.push_back(0);
  }
  if (cmdB5.hasSelfClean) {
    this->m_data.push_back(57);
    this->m_data.push_back(0);
  }
  if (cmdB5.hasOneKeyNoWindOnMe) {
    this->m_data.push_back(66);
    this->m_data.push_back(0);
  }
  if (cmdB5.hasBreeze) {
    this->m_data.push_back(67);
    this->m_data.push_back(0);
  }
  if (cmdB5.hasSmartEye) {
    this->m_data.push_back(48);
    this->m_data.push_back(0);
  }
  if (cmdB5.hasBuzzer) {
    this->m_data.push_back(44);
    this->m_data.push_back(2);
  }
  if (cmdB5.hasAutoClearHumidity || cmdB5.hasHandClearHumidity) {
    this->m_data.push_back(21);
    this->m_data.push_back(0);
  }
  if (cmdB5.hasVerticalWind) {
    this->m_data.push_back(9);
    this->m_data.push_back(0);
  }
  if (cmdB5.hasHorizontalWind) {
    this->m_data.push_back(10);
    this->m_data.push_back(0);
  }
  if (cmdB5.isTwins) {
    this->m_data.push_back(49);
    this->m_data.push_back(2);
  }
  if (cmdB5.isFourDirection) {
    this->m_data.push_back(48);
    this->m_data.push_back(2);
  }
  if (cmdB5.hasJetCool) {
    this->m_data.push_back(103);
    this->m_data.push_back(0);
  }
  if (cmdB5.hasFreshAir) {
    this->m_data.push_back(75);
    this->m_data.push_back(0);
  }
  this->m_data[1] = this->m_data.size() / 2 - 1;
  this->appendCRC();
}

}  // namespace ac
}  // namespace midea
}  // namespace dudanov
