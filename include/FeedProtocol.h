
#ifndef FEED_PROTOCOL_H
#define FEED_PROTOCOL_H

#include "MarketEvent.h"

#include <array>
#include <cstddef>
#include <cstdint>

namespace market::protocol {

constexpr std::size_t MESSAGE_SIZE = 38;

enum class WireEventType : std::uint8_t {
    Add = 1,
    Cancel = 2,
    Modify = 3,
    Trade = 4
};

enum class WireSide : std::uint8_t {
    Buy = 1,
    Sell = 2
};

using EncodedMessage = std::array<std::uint8_t, MESSAGE_SIZE>;

bool writeU64BE(
    EncodedMessage& message,
    std::size_t offset,
    std::uint64_t value
);

bool writeU32BE(
    EncodedMessage& message,
    std::size_t offset,
    std::uint32_t value
);

bool encode(
    const MarketEvent& event,
    EncodedMessage& output
);

bool decode(
    const std::uint8_t* data,
    std::size_t length,
    MarketEvent& output
);

bool readU64BE(
    const std::uint8_t* data,
    std::size_t length,
    std::size_t offset,
    std::uint64_t& value
);

bool readU32BE(
    const std::uint8_t* data,
    std::size_t length,
    std::size_t offset,
    std::uint32_t& value
);


} // namespace market::protocol

#endif
