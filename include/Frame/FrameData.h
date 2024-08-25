#pragma once
#include <cstdint>
#include <vector>

namespace dudanov {
namespace midea {

/**
 * @brief Message body.
 *
 */
class FrameData {
 public:
  /**
   * @brief Default constructor. Data vector is empty.
   */
  FrameData() = default;

  /**
   * @brief Constructs data body from begin and end data iterators.
   *
   * @param begin begin iterator.
   * @param end end iterator.
   */
  FrameData(std::vector<uint8_t>::const_iterator begin, std::vector<uint8_t>::const_iterator end)
      : m_data(begin, end) {}

  /**
   * @brief Init data with initializer list of bytes.
   *
   * @param list list of bytes.
   */
  FrameData(std::initializer_list<uint8_t> list) : m_data(list) {}

  /**
   * @brief Init data with zero bytes.
   *
   * @param size number of zeroes.
   */
  FrameData(uint8_t size) : m_data(size, 0) {}

  /**
   * @brief Move this object to derivied frame data object.
   *
   * @tparam T frame data class type.
   * @return New data frame object.
   */
  template<typename T> T to() { return T{std::move(*this)}; }

  const uint8_t &operator[](size_t idx) const { return this->m_data[idx]; }

  uint8_t &operator[](size_t idx) { return this->m_data[idx]; }

  const uint8_t *data() const { return this->m_data.data(); }

  uint8_t size() const { return this->m_data.size(); }

  bool hasID(uint8_t value) const { return this->m_data[0] == value; }

  bool hasStatus() const { return this->hasID(0xC0); }

  bool hasPowerInfo() const { return this->hasID(0xC1); }

  /**
   * @brief Calculates and adds a checksum.
   *
   */
  void appendCRC() { this->m_data.push_back(this->m_calcCRC()); }

  /**
   * @brief Updates the checksum (last byte).
   *
   */
  void updateCRC() {
    this->m_data.pop_back();
    this->appendCRC();
  }

  /**
   * @brief Verifies the checksum.
   *
   * @return `true` if checksum is correct.
   */
  bool hasValidCRC() const { return this->m_calcCRC() == 0; }

  /**
   * @brief Appends variadic number of integer arguments.
   *
   * @param data first argument.
   * @param ...args other arguments.
   */
  template<typename T, typename... Args> void append(const T &data, Args... args) {
    this->append(data);
    this->append(args...);
  }

  /**
   * @brief Appends integer data.
   *
   * @tparam T integer data type.
   * @param data data to append.
   */
  template<typename T> void append(const T &data) {
    for (size_t i = 0; i < sizeof(T); ++i)
      this->m_data.push_back(data >> (i * 8));
  }

 protected:
  /**
   * @brief Data vector.
   *
   */
  std::vector<uint8_t> m_data;

  /**
   * @brief Static function for generate message ID.
   *
   * @return Message ID.
   */
  static uint8_t m_getID();

  /**
   * @brief Calculate checksum.
   *
   * @return Checksum.
   */
  uint8_t m_calcCRC() const;

  /**
   * @brief Get value from data body.
   *
   * @param idx byte index.
   * @param mask value mask (default: 0xFF).
   * @param rshift number of bits right shift (default: 0).
   * @return Value.
   */
  uint8_t m_getValue(uint8_t idx, uint8_t mask = 255, uint8_t rshift = 0) const {
    return (this->m_data[idx] >> rshift) & mask;
  }

  /**
   * @brief Get bit value from data body.
   *
   * @param idx byte index.
   * @param bit bit number.
   * @return Bit state.
   */
  bool m_getBit(uint8_t idx, uint8_t bit) const { return this->m_getValue(idx, 1, bit); }

  /**
   * @brief Set value to data body.
   *
   * @param idx byte index.
   * @param value value without shift.
   * @param mask value mask without bits shift (default: 0xFF). Used to clear destination bits.
   * @param lshift number of bits to left shift value and mask.
   */
  void m_setValue(uint8_t idx, uint8_t value, uint8_t mask = 255, uint8_t lshift = 0) {
    this->m_data[idx] &= ~(mask << lshift);
    this->m_data[idx] |= (value << lshift);
  }

  /**
   * @brief Set or clear bitmask.
   *
   * @param idx byte index.
   * @param state if `true` bitmask is set, else clear.
   * @param mask bitmask.
   */
  void m_setMask(uint8_t idx, bool state, uint8_t mask = 255) { this->m_setValue(idx, state ? mask : 0, mask); }

  /**
   * @brief Set bit state.
   *
   *
   * @param idx byte index.
   * @param bit bit number.
   * @param state bit state.
   */
  void m_setBit(uint8_t idx, uint8_t bit, bool state) { this->m_setMask(idx, state, 1 << bit); }
};

}  // namespace midea
}  // namespace dudanov
