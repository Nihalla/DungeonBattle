#pragma once

#include "cstring"
#include "string"
#include "vector"

enum class PacketID : int
{
  INVALID = -1,
  ENEMY_UPDATE,
  BOOL,
  READY,
  UNIT,
  PLAYER_ID,
  UNIT_REQUEST,
  UNIT_BUNDLE
};

/*
 * MyPacket is a custom class designed to stream bytes of data.
 * This can be used to stream to disk or network IO subsystems.
 */

class MyPacket
{
 public:
  using bytes = std::vector<std::byte>;

 private:
  size_t read_pos = 0;               // current read position
  std::vector<std::byte> _data{ 0 }; // the packet data

 public:
  /*
   * Constructor that allows an initial size to be specified.
   * Doing this will reserve the memory required, but will
   * not resize the vector, meaning it will be empty.
   * @param [in] capacity The initial size to reserve for the buffer
   */
  explicit MyPacket(size_t capacity = 1024) { _data.reserve(capacity); }

  // you could use these to create constructors that accepts bytes
  [[maybe_unused]] MyPacket(const bytes& /*src*/, size_t /*length*/) {}
  [[maybe_unused]] MyPacket(bytes&& /*unused*/, size_t /*unused*/) {}

  // disable copying of packet, though you could enable if you have good reason
  MyPacket(const MyPacket&) = delete;

  // simple getters to access the data
  [[nodiscard]] bool isEnd() const { return read_pos >= _data.size(); }
  [[nodiscard]] const bytes& data() const { return _data; }
  [[nodiscard]] bytes& data() { return _data; }
  [[nodiscard]] size_t length() const { return _data.size(); }

  // resets the read head position allowing the buffer to be re-read
  [[maybe_unused]] void resetReadHead() { read_pos = 0; }

  /*
   * Streams data directly into the buffer. It will use sizeof
   * to determine the amount of data to restore. This will not
   * work with pointers.
   * @param [in] data Templated data type to stream into the buffer
   */
  template<typename T>
  MyPacket& operator<<(const T& data)
  {
    static_assert(!std::is_pointer_v<T>, "no pointers please");
    auto size           = sizeof(data);
    const auto* as_byte = reinterpret_cast<const std::byte*>(&data);
    _data.insert(_data.end(), as_byte, as_byte + size);
    return *this;
  }

  /*
   * Reads from the buffer and streams the data into the user provided
   * type. If the type is the wrong size then this will lead to
   * undefined behaviour. Again this does not support pointers.
   * @param [out] data Destination for buffer to stream in to
   */
  template<typename T>
  MyPacket& operator>>(T& data)
  {
    static_assert(!std::is_pointer_v<T>, "no pointers please");
    auto size    = sizeof(T);
    auto as_type = reinterpret_cast<T*>(&_data[read_pos]);
    data         = *as_type;
    read_pos += size;
    return *this;
  }

  /*
   * Overload for streaming strings. This is needed because strings
   * store data dynamically on the heap. This function also preserves
   * the null terminator.
   * @param [in] str String to place in the packet's data buffer
   */
  MyPacket& operator<<(const std::string& str)
  {
    const auto* str_data = reinterpret_cast<const std::byte*>(str.data());
    auto length          = str.length() + 1; // +1 for null terminator
    _data.insert(_data.end(), str_data, str_data + length);
    return *this;
  }

  /*
   * Calls the rvalue version of the string streaming operator
   * @param [in] str String to place in the packet's data buffer
   */
  MyPacket& operator<<(std::string&& str) { return *this << str; }

  /*
   * Reads a null terminated string from the buffer. The string
   * must be null terminated for this to work.
   * @param [out] str Destination string to store buffer contents in
   */
  MyPacket& operator>>(std::string& str)
  {
    auto* as_char = reinterpret_cast<std::byte*>(&_data[read_pos]);
    auto length   = std::strlen(reinterpret_cast<const char*>(as_char));
    str           = std::string(reinterpret_cast<const char* const>(as_char), length);
    read_pos += length;
    return *this;
  }
};