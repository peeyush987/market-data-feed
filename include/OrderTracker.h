#ifndef ORDER_TRACKER_H
#define ORDER_TRACKER_H

#include "MarketEvent.h"
#include "OrderState.h"

#include <cstddef>
#include <cstdint>
#include <unordered_map>

namespace market {

class OrderTracker {
public:
    bool apply(const MarketEvent& event);

    bool contains(std::uint64_t orderId) const;
    std::size_t activeOrderCount() const;

private:
    std::unordered_map<std::uint64_t, SimulatedOrder> activeOrders_;
};

} // namespace market

#endif