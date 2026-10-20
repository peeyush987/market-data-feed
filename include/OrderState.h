
#ifndef ORDER_STATE_H
#define ORDER_STATE_H

#include "MarketEvent.h"

namespace market {

struct SimulatedOrder {
    std::uint64_t orderId = 0;
    Side side = Side::Buy;
    std::int64_t priceTicks = 0;
    std::uint32_t remainingQuantity = 0;
};

} // namespace market

#endif
