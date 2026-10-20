
#ifndef MARKET_EVENT_H
#define MARKET_EVENT_H

#include <cstdint>

namespace market {

enum class EventType : std::uint8_t {
    Add,
    Cancel,
    Modify,
    Trade
};

enum class Side : std::uint8_t {
    Buy,
    Sell
};

struct MarketEvent {
    std::uint64_t sequence = 0;
    EventType type = EventType::Add;

    std::uint64_t orderId = 0;
    std::uint64_t relatedOrderId = 0;

    Side side = Side::Buy;
    std::int64_t priceTicks = 0;
    std::uint32_t quantity = 0;
};

} // namespace market

#endif
