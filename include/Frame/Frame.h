#pragma once
#include <cstdint>
#include <vector>
#include <string>

#include "Frame/FrameData.h"

namespace dudanov {
namespace midea {

class Frame {
 public:
  explicit Frame() = default;

  /**
   * @brief Makes `Frame` instance from parameters.
   *
   * @param applianceID appliance ID.
   * @param protocolID protocol ID.
   * @param typeID frame type ID.
   * @param data frame data body.
   * @return `Frame` instance.
   */
  static Frame make(uint8_t applianceID, uint8_t protocolID, uint8_t typeID, const FrameData &data) {
    return Frame{applianceID, protocolID, typeID, data};
  }

  /**
   * @brief Extracts data body from frame. Frame MUST BE full and valid.
   *
   * @return Frame data body.
   */
  FrameData getData() const;

  /**
   * @brief Removes and replaces frame data body.
   *
   * @param data frame data body.
   */
  void setData(const FrameData &data);

  /**
   * @brief Get frame raw data pointer.
   *
   * @return Raw data pointer.
   */
  const uint8_t *data() const { return m_data.data(); }

  /**
   * @brief Get raw data size.
   *
   * @return Raw data size.
   */
  size_t size() const { return m_data.size(); }

  /**
   * @brief Set frame type.
   *
   * @param value frame type.
   */
  void setType(uint8_t value) { m_data[OFFSET_TYPE] = value; }

  /**
   * @brief Check frame type.
   *
   * @param value frame type.
   * @return `true` if frame has specified type.
   */
  bool hasType(uint8_t value) const { return m_data[OFFSET_TYPE] == value; }

  void setProtocol(uint8_t value) { m_data[OFFSET_PROTOCOL] = value; }

  uint8_t getProtocol() const { return m_data[OFFSET_PROTOCOL]; }

  /**
   * @brief Print frame as hex string.
   *
   * @return Hex string.
   */
  std::string toString() const;

  /**
   * @brief Deserializes frame byte by byte. Caller is responsible for clearing frame after handling.
   *
   * @param data byte to process.
   * @return `true` if frame deserializing is complete and it ready for handling.
   */
  bool deserialize(const uint8_t &data);

  /**
   * @brief Clears whole frame.
   *
   */
  void clear() { m_data.clear(); }

 protected:
  /**
   * @brief Raw data vector.
   */
  std::vector<uint8_t> m_data;

  /**
   * @brief Trim frame data body.
   *
   */
  void m_trimData() { m_data.erase(m_data.begin() + OFFSET_DATA, m_data.end()); }

  void m_appendData(const FrameData &data) {
    std::copy(data.m_data.begin(), data.m_data.end(), std::back_inserter(m_data));
  }

  /**
   * @brief Length of frame (without checksum).
   *
   * @return Length.
   */
  uint8_t m_len() const { return m_data[OFFSET_LENGTH]; }

  /**
   * @brief Calculates checksum and finalize frame by appending it to the end.
   *
   */
  void m_appendCS() { m_data.push_back(m_calcCS()); }

  /**
   * @brief Calculate checksum.
   *
   * @return Checksum.
   */
  uint8_t m_calcCS() const;

  static const uint8_t START_BYTE = 0xAA;
  static const uint8_t OFFSET_START = 0;
  static const uint8_t OFFSET_LENGTH = 1;
  static const uint8_t OFFSET_APPTYPE = 2;
  static const uint8_t OFFSET_PROTOCOL = 8;
  static const uint8_t OFFSET_TYPE = 9;
  static const uint8_t OFFSET_DATA = 10;

  explicit Frame(uint8_t applianceID, uint8_t protocolID, uint8_t typeID, const FrameData &data);
};

}  // namespace midea
}  // namespace dudanov
